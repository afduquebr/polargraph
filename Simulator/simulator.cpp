// Polargraph Simulation library
// Last Update: December 14th, 2023.
// Andrés Felipe Duque Bran


#include "simulator.h"
#include <iostream>

using namespace std;

// Definition of the constructor
Polargraph::Polargraph(float a, float b) {
  // Defining initial position of the pointer
  position.x = a;
  position.y = b;
  // Determination of resolution
  resolution = sqrt(2) * radius * M_PI /100;
}

// Set speed for the displacement
// void Polargraph::speed(int v) {
//   // Set same speed for both engines
//   // v is set in rpm
//   motor1.setSpeed(v);
//   motor2.setSpeed(v);
// }

// Method for getting to a certain point in grid
bool Polargraph::initPosition(float x0, float y0) {
  // Bool variable to control position of pointer wrt target
  bool stop = 0;
  // Move in Y and then in X
  if (abs(y0 - position.y) > resolution) {
    moveInY(y0);
  } else if (abs(x0 - position.x) > resolution) {
    moveInX(x0);
  } else {
    stop = 1;
  }
  // Returns True when movement in X is finished
  return stop;
}

// Method for horizontal displacement
bool Polargraph::moveInX(float newX) {
  // Bool variables to control x position of pointer wrt target
  bool stop = 0;
  // Warning in case desired position is out of Canvas
  if ( (newX < - (width / 2)) && ((width / 2) < newX) ) {
    cout << " FATAL ERROR: Pointer position outside of Canvas\n";
      stop = 1;
  } else {
    // if ((newX - position.x) > 0) {
    //   // Move in +X direction
    //   motor1.step(1, FORWARD, SINGLE);
    //   motor2.step(1, FORWARD, SINGLE);
    // } else if ((newX - position.x) < 0) {
    //   // Move in -X direction
    //   motor1.step(1, BACKWARD, SINGLE);
    //   motor2.step(1, BACKWARD, SINGLE);
    // }
    // Redefine pointer position after step
    if (abs(newX - position.x) > resolution) {
      position.x += 2 * resolution * copysign(1.0, (newX - position.x));
    } else {
      stop = 1;
    }
    // Returns True when target position is reached
  }
  return stop;
}

// Method for vertical displacement
bool Polargraph::moveInY(float newY) {
  // Bool variables to control y position of pointer wrt target
  bool stop = 0;
  // Warning in case desired position is out of Canvas
  if ( (newY < 0.0) && (height < newY) )  {
    cout << " FATAL ERROR: Pointer position outside of Canvas\n";
    stop = 1;
  } else {
    // if ((newY - position.y) < 0) {
    //   // Move in -Y direction
    //   motor2.step(1, FORWARD, SINGLE);
    //   motor1.step(1, BACKWARD, SINGLE);
    // } else if ((newY - position.y) > 0) {
    //   // Move in +Y direction
    //   motor2.step(1, BACKWARD, SINGLE);
    //   motor1.step(1, FORWARD, SINGLE);
    // }
    // Redefine pointer position after step
    if (abs(newY - position.y) > resolution) {
      position.y += 2 * resolution * copysign(1.0, (newY - position.y));
    } else {
      stop = 1;
    }
    // Returns True when target position is reached
  }
  return stop;
}

bool Polargraph::square(float x, float y, float d) {
  // Bool variables to control position of pointer wrt target
  bool change, stop;
  if (sq_state == 0) {
    // Get to right top of the square
    change = initPosition(x, y);
    if (change == 1) sq_state += 1;
  } else if (sq_state == 1) {
    // Draw upper horizontal line
    change = moveInX(x + d);
    if (change == 1) sq_state += 1;
  } else if (sq_state == 2) {
    // Draw right vertical line
    change = moveInY(y + d);
    if (change == 1) sq_state += 1;
  } else if (sq_state == 3) {
    // Draw lower horizontal line
    change = moveInX(x);
    if (change == 1) sq_state += 1;
  } else {
    // Draw left vertical line
    stop = moveInY(y);
    sq_state = 0;
  }
  return stop;
}

// Method for going back to default pointer position
bool Polargraph::restPosition() {
  return initPosition(0.0, 0.0);
}

// Method for returning current position in the grid
coordenates Polargraph::getPosition() {
  return position;
}

bool Polargraph::move(float newX, float newY, bool draw) {
  // Bool variable to control position of pointer wrt target
  bool stop = 0;
  // Warning in case desired position is out of Canvas
  if ( (newX < - (width / 2)) && ((width / 2) < newX) && (newY < 0.0) && (height < newY) )  {
    cout << " FATAL ERROR: Pointer position outside of Canvas\n";
    stop = 1;
  } else {
    if (abs(newX - position.x) + abs(newY - position.y) > limit) {
      int min = minimum(newX, newY);
      if (min == 0) {
        position.x -= resolution;
        position.y -= resolution;
      } else if (min == 1) {
        position.x += resolution;
        position.y -= resolution;
      } else if (min == 2) {
        position.x += resolution;
        position.y += resolution;
      } else if (min == 3) {
        position.x -= resolution;
        position.y += resolution;
      } 
      stop = 0;
    } else {
      fit = 1;
      stop = 1;
    }
  }
  return stop;
}

void Polargraph::linearFit(float x, float y){
  if (abs(y - position.y) <= abs(x - position.x)) {
    fit_parameters[0] = (y - position.y) / (x - position.x);
    fit_parameters[1] = y - fit_parameters[0] * x;
    fit_parameters[2] = 0;
  } else {
    fit_parameters[0] = (x - position.x) / (y - position.y);
    fit_parameters[1] = x - fit_parameters[0] * y;
    fit_parameters[2] = 1;
  }
}

float Polargraph::distance(float x1, float y1, float x2, float y2) {
  return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

int Polargraph::minimum(float x, float y) {
  float diagonal[4];
  if (fit == 1) {
    linearFit(x, y);
    fit = 0;
  }
  if (fit_parameters[2] == 0) {
    diagonal[0] = distance(0, position.y - resolution, 0, fit_parameters[0] * (position.x - resolution) + fit_parameters[1]);
    diagonal[1] = distance(0, position.y - resolution, 0, fit_parameters[0] * (position.x + resolution) + fit_parameters[1]);
    diagonal[2] = distance(0, position.y + resolution, 0, fit_parameters[0] * (position.x + resolution) + fit_parameters[1]);
    diagonal[3] = distance(0, position.y + resolution, 0, fit_parameters[0] * (position.x - resolution) + fit_parameters[1]);
  } else {
    diagonal[0] = distance(position.x - resolution, 0, fit_parameters[0] * (position.y - resolution) + fit_parameters[1], 0);
    diagonal[1] = distance(position.x + resolution, 0, fit_parameters[0] * (position.y - resolution) + fit_parameters[1], 0);
    diagonal[2] = distance(position.x + resolution, 0, fit_parameters[0] * (position.y + resolution) + fit_parameters[1], 0);
    diagonal[3] = distance(position.x - resolution, 0, fit_parameters[0] * (position.y + resolution) + fit_parameters[1], 0);
  }
  float dist[4] = {
    distance(position.x - resolution, position.y - resolution, x, y),
    distance(position.x + resolution, position.y - resolution, x, y),
    distance(position.x + resolution, position.y + resolution, x, y),
    distance(position.x - resolution, position.y + resolution, x, y),
  };
  int min = 0, min1 = 0, min2 = 1;
  if (dist[min1] > dist[1]) {
    min2 = min1;
    min1 = 1;
  }
  if (dist[min1] > dist[2]) {
    min2 = min1;
    min1 = 2; 
  } else if (dist[min2] > dist[2]) {
    min2 = 2;
  }
  if (dist[min1] > dist[3]) {
    min2 = min1;
    min1 = 3;
  } else if (dist[min2] > dist[3]) {
    min2 = 3;
  }
  if(diagonal[min1] < diagonal[min2]) {
    min = min1;
  } else {
    min = min2;
  }
  // cout << "min = " << min << endl;
  // cout << "min1 = " << min1 << endl;
  // cout << "min2 = " << min2 << endl;
  // cout << "dist[0] = " << dist[0] << endl;
  // cout << "dist[1] = " << dist[1] << endl;
  // cout << "dist[2] = " << dist[2] << endl;
  // cout << "dist[3] = " << dist[3] << endl;
  // cout << "fit_parameters[2] = " << fit_parameters[2] << endl;
  // cout << "diagonal[min1] = " << diagonal[min1] << endl;
  // cout << "diagonal[min2] = " << diagonal[min2] << endl;
  return min;
}



