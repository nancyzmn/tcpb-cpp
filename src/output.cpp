/** \file output.cpp
 *  \brief Implementation of Output class
 */

#include <string>

#include <google/protobuf/util/message_differencer.h>

#include "output.h"

#include "terachem_server.pb.h"
using terachem_server::JobOutput;

namespace TCPB {

void Output::GetEnergy(double &energy,
  int state,
  int mult) const
{
  // TODO: Add state/mult logic
  energy = pb_.energy(0);
}

void Output::SetEnergy(double energy,
  int state,
  int mult)
{
  // TODO: Add state/mult logic
  pb_.add_energy(energy);
}

void Output::GetGradient(double *qmgradient,
  double *mmgradient,
  int state,
  int mult) const
{
  // TODO: Add state/mult logic
  int qmgrad_size = pb_.gradient_size();
  memcpy(qmgradient, pb_.gradient().data(), qmgrad_size * sizeof(double));
  if (mmgradient != nullptr) {
    int mmgrad_size = pb_.mmatom_gradient_size();
    memcpy(mmgradient, pb_.mmatom_gradient().data(), mmgrad_size * sizeof(double));
  }
}

bool Output::IsApproxEqual(const Output &other) const
{
  using namespace google::protobuf::util;
  return MessageDifferencer::ApproximatelyEquals(pb_, other.pb_);
}

} // end namespace TCPB
