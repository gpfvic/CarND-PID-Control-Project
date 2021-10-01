#include "PID.h"
#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

/*
* TODO: Complete the PID class.
*/

PID::PID() {}

PID::~PID() {}

void PID::Init(double Kp_, double Ki_, double Kd_)
{
  p = {Kp_, Kd_, Ki_}; // P D I
  errors = {0., 0., 0.}; // p d i
}

void PID::UpdateError(double cte)
{
  errors[1] = cte - errors[0]; // d error
  errors[0] = cte; // p_error
  errors[2] += cte; // i error
}

double PID::TotalError()
{
  // return -Kp * p_error - Kd * d_error - Ki * i_error;
  return -p[0]*errors[0] - p[1]*errors[1] - p[2]*errors[2];
}

void PID::twiddle_init(){
  dp = {0.1*p[0], 0.1*p[1], 0.1*p[2]}; // P D I
  step = 0;
  iteration = 1;
  param_index = 0;
  total_error = 0;
  best_error = 99999999;
}

void PID::twiddle_first(){
  if(step!=0){
    return ;
  }
  p[param_index] += dp[param_index];
  step =1;
}

void PID::twiddle_tune(double cte){
  if(step !=1){
    return;
  }
  
  if(total_error < best_error){
    best_error = total_error;
    dp[param_index] *= 1.1;
  }else{
    p[param_index] -= 2*dp[param_index];
  }
  step = 2;
}

void PID::twiddle_eval(double cte){
  if(step!=2){
    return;
  }

  if(total_error < best_error){
    best_error = total_error;
    dp[param_index] *= 1.1;
  }else{
    p[param_index] += dp[param_index];
    dp[param_index] *= 0.9;
  }

  // for the next iteration
  param_index = (param_index + 1) % p.size();
  step = 0;
 
 total_error =0;
}


