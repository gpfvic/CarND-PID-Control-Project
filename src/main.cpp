#include <math.h>
#include <uWS/uWS.h>
#include <iostream>
#include <string>
#include "json.hpp"
#include "PID.h"

// for convenience
using nlohmann::json;
using std::string;

// For converting back and forth between radians and degrees.
constexpr double pi() { return M_PI; }
double deg2rad(double x) { return x * pi() / 180; }
double rad2deg(double x) { return x * 180 / pi(); }

// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
string hasData(string s) {
  auto found_null = s.find("null");
  auto b1 = s.find_first_of("[");
  auto b2 = s.find_last_of("]");
  if (found_null != string::npos) {
    return "";
  }
  else if (b1 != string::npos && b2 != string::npos) {
    return s.substr(b1, b2 - b1 + 1);
  }
  return "";
}

int main() {
  uWS::Hub h;

  PID pid_s;
  PID pid_t;
  /**
   * TODO: Initialize the pid variable.
   */
  pid_s.Init(0.1, 0.0001, 3.0);
  pid_t.Init(0.3, 0, 0.02);

  pid_s.twiddle_init();
  pid_t.twiddle_init();
  
  
  h.onMessage([&pid_s, &pid_t](uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, 
                     uWS::OpCode opCode) {
    // "42" at the start of the message means there's a websocket message event.
    // The 4 signifies a websocket message
    // The 2 signifies a websocket event
    if (length && length > 2 && data[0] == '4' && data[1] == '2') {
      auto s = hasData(string(data).substr(0, length));

      if (s != "") {
        auto j = json::parse(s);

        string event = j[0].get<string>();

        if (event == "telemetry") {
          // j[1] is the data JSON object
          double cte = std::stod(j[1]["cte"].get<string>());
          // double speed = std::stod(j[1]["speed"].get<string>());
          // double angle = std::stod(j[1]["steering_angle"].get<string>());
          double steer_value, throttle_value;
          /**
           * TODO: Calculate steering value here, remember the steering value is
           *   [-1, 1].
           * NOTE: Feel free to play around with the throttle and speed.
           *   Maybe use another PID controller to control the speed!
           */
          
          pid_s.UpdateError(cte);
          pid_t.UpdateError(fabs(cte));
          
          int training_steps = 20;
          pid_s.total_error += pow(cte,2);
          if(pid_s.iteration%training_steps ==0){
            cout<<"\n\nDEBUG-----------------------------------"<<endl;
            cout<<"Steer Iteration: "<<pid_s.iteration<<"\tError Best vs Curr = ["<<pid_s.best_error<<", "<<pid_s.total_error<<"]"<<endl;
            pid_s.twiddle_first();
            pid_s.twiddle_tune(cte);
            pid_s.twiddle_eval(cte);
            cout<<"Steer P = ["<<pid_s.p[0]<<", "<<pid_s.p[1]<<", "<<pid_s.p[2]<<"]\t\tDP = ["<<pid_s.dp[0]<<", "<<pid_s.dp[1]<<", "<<pid_s.dp[2]<<"]"<<endl;
          }
          pid_s.iteration += 1;

          pid_t.total_error += pow(cte,2);
          if(pid_t.iteration%training_steps==0){
            cout<<"Throt Iteration: "<<pid_t.iteration<<"\tError Best vs Curr = ["<<pid_t.best_error<<", "<<pid_t.total_error<<"]"<<endl;
            pid_t.twiddle_first();
            pid_t.twiddle_tune(cte);
            pid_t.twiddle_eval(cte);
            cout<<"Throt P = ["<<pid_t.p[0]<<", "<<pid_t.p[1]<<", "<<pid_t.p[2]<<"]\t\tDP = ["<<pid_t.dp[0]<<", "<<pid_t.dp[1]<<", "<<pid_t.dp[2]<<"]"<<endl;
          }
          pid_t.iteration += 1;


          steer_value = pid_s.TotalError();
          throttle_value = 0.8 + pid_t.TotalError();
          


          json msgJson;
          msgJson["steering_angle"] = steer_value;
          msgJson["throttle"] = throttle_value;
          auto msg = "42[\"steer\"," + msgJson.dump() + "]";
          // std::cout << msg << std::endl;
          ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
        }  // end "telemetry" if
      } else {
        // Manual driving
        string msg = "42[\"manual\",{}]";
        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      }
    }  // end websocket message if
  }); // end h.onMessage

  h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
    std::cout << "Connected!!!" << std::endl;
  });

  h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code, 
                         char *message, size_t length) {
    ws.close();
    std::cout << "Disconnected" << std::endl;
  });

  int port = 4567;
  if (h.listen(port)) {
    std::cout << "Listening to port " << port << std::endl;
  } else {
    std::cerr << "Failed to listen to port" << std::endl;
    return -1;
  }
  
  h.run();
}