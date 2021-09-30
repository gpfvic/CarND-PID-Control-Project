#ifndef PID_H
#define PID_H
#include <vector>

using namespace std;

class PID {
 public:
  /**
   * Constructor
   */
  PID();

  /**
   * Destructor.
   */
  virtual ~PID();

  /**
   * Initialize PID.
   * @param (Kp_, Ki_, Kd_) The initial PID coefficients
   */
  void Init(double Kp_, double Ki_, double Kd_);

  /**
   * Update the PID error variables given cross track error.
   * @param cte The current cross track error
   */
  void UpdateError(double cte);

  /**
   * Calculate the total PID error.
   * @output The total PID error
   */
  double TotalError();

  /**
   * tuning the pid parameters
   */
  void twiddle_tune(double cte);

  void twiddle_eval(double cte);

  void twiddle_first();

  void twiddle_init();

  /**
   * PID Errors
   */
  vector<double> errors; // [p_error, d_error, i_error]

  /**
   * PID Coefficients
   */ 
  vector<double> p; //[ Kp, Kd, Ki]

  /**
   * PID parameters tuning: twiddle 
   */
  int step;
  int iteration;
  vector<double> dp;
  double total_error;
  double best_error;
  int param_index;
  
};

#endif  // PID_H