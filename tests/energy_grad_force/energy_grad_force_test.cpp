/** \file energy_grad_force_test.cpp
 *  \brief C++ version of energy_grad_force_test.py
 *  \author Stefan Seritan <sseritan@stanford.edu>
 *  \date Aug 2017
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "tcpb.h"

bool FuzzyEqual(const double* arr1,
                const double* arr2,
                const int arrSize,
                const double tol) {
  bool fail = false;

  for (int i = 0; i < arrSize; i++) {
    if (abs(arr1[i] - arr2[i]) > tol) {
      fail = true;
      break;
    }
  }

  return !fail;
}

int main(int argc, char** argv) {
  if (argc != 3) {
    printf("Usage: %s host port\n", argv[0]);
  }

  int port = atoi(argv[2]);
  TCPBClient* TC = new TCPBClient(argv[1], port);

  TC->Connect();

  // Set up system
  int num_atoms = 3;
  const char* atoms[3] = {"O", "H", "H"};
  double geom[9] = {0.00000,  0.00000, -0.12948,
                    0.00000, -1.49419,  1.02744,
                    0.00000,  1.49419,  1.02744};
  TC->SetAtoms(atoms, num_atoms);
  TC->SetCharge(0);
  TC->SetSpinMult(1);
  TC->SetClosed(true);
  TC->SetRestricted(true);
  TC->SetMethod("pbe0");
  TC->SetBasis("6-31g");

  // Expected answers
  double tol = 1e-5;
  double expected_energy = -76.300505;
  double expected_grad[9] = {  0.0000002903,    0.0000000722,   -0.033101313,
                              -0.0000000608,   -0.0141756697,    0.016550727,
                              -0.0000002294,    0.0141755976,    0.016550585};

  // Run tests
  double energy;
  double* grad = new double[9];

  TC->ComputeEnergy(geom, num_atoms, false, energy);
  if (!FuzzyEqual(&energy, &expected_energy, 1, tol)) {
    printf("Failed energy test\n");
    printf("Expected energy: %lf\nGot energy: %lf\n", expected_energy, energy);
    return 1;
  }

  TC->ComputeGradient(geom, num_atoms, false, energy, grad);
  if (!FuzzyEqual(&energy, &expected_energy, 1, tol) || !FuzzyEqual(grad, expected_grad, 3*num_atoms, tol)) {
    printf("Failed gradient test\n");
    printf("Expected energy: %lf\nGot energy: %lf\n", expected_energy, energy);
    printf("Expected gradient: \n");
    for (int i = 1; i < num_atoms; i++) {
      printf("%lf %lf %lf\n", expected_grad[i], expected_grad[i+1], expected_grad[i+2]);
    }
    printf("Got gradient: \n");
    for (int i = 1; i < num_atoms; i++) {
      printf("%lf %lf %lf\n", grad[i], grad[i+1], grad[i+2]);
    }
    return 1;
  }

  TC->ComputeForces(geom, num_atoms, false, energy, grad);
  for (int i = 0; i < 3*num_atoms; i++) {
    grad[i] *= -1.0;
  }
  if (!FuzzyEqual(&energy, &expected_energy, 1, tol) || !FuzzyEqual(grad, expected_grad, 3*num_atoms, tol)) {
    printf("Failed force test\n");
    printf("Expected energy: %lf\nGot energy: %lf\n", expected_energy, energy);
    printf("Expected gradient (-1*force): \n");
    for (int i = 1; i < num_atoms; i++) {
      printf("%lf %lf %lf\n", expected_grad[i], expected_grad[i+1], expected_grad[i+2]);
    }
    printf("Got gradient (-1*force): \n");
    for (int i = 1; i < num_atoms; i++) {
      printf("%lf %lf %lf\n", grad[i], grad[i+1], grad[i+2]);
    }
    return 1;
  }

  // Memory Management
  delete TC; //Handles disconnect
  delete grad;

  return 0;
}
