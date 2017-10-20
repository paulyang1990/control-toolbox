/***********************************************************************************
Copyright (c) 2017, Michael Neunert, Markus Giftthaler, Markus Stäuble, Diego Pardo,
Farbod Farshidian. All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name of ETH ZURICH nor the names of its contributors may be used
      to endorse or promote products derived from this software without specific
      prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL ETH ZURICH BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************************************/

#pragma once

namespace ct {
namespace optcon {


template <size_t STATE_DIM, size_t CONTROL_DIM, typename SCALAR_EVAL, typename SCALAR>
TermQuadMult<STATE_DIM, CONTROL_DIM, SCALAR_EVAL, SCALAR>::TermQuadMult(const state_matrix_t& Q,
	const control_matrix_t& R)
	: Q_(Q), R_(R)
{
	x_ref_.setZero();  // default values
	u_ref_.setZero();
}

template <size_t STATE_DIM, size_t CONTROL_DIM, typename SCALAR_EVAL, typename SCALAR>
TermQuadMult<STATE_DIM, CONTROL_DIM, SCALAR_EVAL, SCALAR>::TermQuadMult()
{
	Q_.setIdentity();  // default values
	R_.setIdentity();
	x_ref_.setZero();
	u_ref_.setZero();
}

template <size_t STATE_DIM, size_t CONTROL_DIM, typename SCALAR_EVAL, typename SCALAR>
TermQuadMult<STATE_DIM, CONTROL_DIM, SCALAR_EVAL, SCALAR>::TermQuadMult(const state_matrix_t& Q,
	const control_matrix_t& R,
	const core::StateVector<STATE_DIM, SCALAR_EVAL>& x_ref,
	core::ControlVector<CONTROL_DIM, SCALAR_EVAL>& u_ref)
	: Q_(Q), R_(R), x_ref_(x_ref), u_ref_(u_ref)
{
}

template <size_t STATE_DIM, size_t CONTROL_DIM, typename SCALAR_EVAL, typename SCALAR>
TermQuadMult<STATE_DIM, CONTROL_DIM, SCALAR_EVAL, SCALAR>::TermQuadMult(const TermQuadMult& arg)
	: TermBase<STATE_DIM, CONTROL_DIM, SCALAR_EVAL, SCALAR>(arg)
	, Q_(arg.Q_)
	, R_(arg.R_)
	, x_ref_(arg.x_ref_)
	, u_ref_(arg.u_ref_)
{
}

template <size_t STATE_DIM, size_t CONTROL_DIM, typename SCALAR_EVAL, typename SCALAR>
TermQuadMult<STATE_DIM, CONTROL_DIM, SCALAR_EVAL, SCALAR>*
TermQuadMult<STATE_DIM, CONTROL_DIM, SCALAR_EVAL, SCALAR>::clone() const
{
	return new TermQuadMult(*this);
}

template <size_t STATE_DIM, size_t CONTROL_DIM, typename SCALAR_EVAL, typename SCALAR>
TermQuadMult<STATE_DIM, CONTROL_DIM, SCALAR_EVAL, SCALAR>::~TermQuadMult()
{
}

template <size_t STATE_DIM, size_t CONTROL_DIM, typename SCALAR_EVAL, typename SCALAR>
void TermQuadMult<STATE_DIM, CONTROL_DIM, SCALAR_EVAL, SCALAR>::setWeights(const state_matrix_double_t& Q,
	const control_matrix_double_t& R)
{
	Q_ = Q.template cast<SCALAR_EVAL>();
	R_ = R.template cast<SCALAR_EVAL>();
}

template <size_t STATE_DIM, size_t CONTROL_DIM, typename SCALAR_EVAL, typename SCALAR>
void TermQuadMult<STATE_DIM, CONTROL_DIM, SCALAR_EVAL, SCALAR>::setStateAndControlReference(
	const core::StateVector<STATE_DIM>& x_ref,
	core::ControlVector<CONTROL_DIM>& u_ref)
{
	x_ref_ = x_ref.template cast<SCALAR_EVAL>();
	;
	u_ref_ = u_ref.template cast<SCALAR_EVAL>();
	;
}

template <size_t STATE_DIM, size_t CONTROL_DIM, typename SCALAR_EVAL, typename SCALAR>
template <typename SC>
SC TermQuadMult<STATE_DIM, CONTROL_DIM, SCALAR_EVAL, SCALAR>::evalLocal(const Eigen::Matrix<SC, STATE_DIM, 1>& x,
	const Eigen::Matrix<SC, CONTROL_DIM, 1>& u,
	const SC& t)
{
	Eigen::Matrix<SC, STATE_DIM, 1> xDiff = (x - x_ref_.template cast<SC>());
	Eigen::Matrix<SC, CONTROL_DIM, 1> uDiff = (u - u_ref_.template cast<SC>());

	return (xDiff.transpose() * Q_.template cast<SC>() * xDiff)(0, 0) *
		   (uDiff.transpose() * R_.template cast<SC>() * uDiff)(0, 0);
}

template <size_t STATE_DIM, size_t CONTROL_DIM, typename SCALAR_EVAL, typename SCALAR>
SCALAR TermQuadMult<STATE_DIM, CONTROL_DIM, SCALAR_EVAL, SCALAR>::evaluate(const Eigen::Matrix<SCALAR, STATE_DIM, 1>& x,
	const Eigen::Matrix<SCALAR, CONTROL_DIM, 1>& u,
	const SCALAR& t)
{
	return evalLocal<SCALAR>(x, u, t);
}

template <size_t STATE_DIM, size_t CONTROL_DIM, typename SCALAR_EVAL, typename SCALAR>
ct::core::ADCGScalar TermQuadMult<STATE_DIM, CONTROL_DIM, SCALAR_EVAL, SCALAR>::evaluateCppadCg(
	const core::StateVector<STATE_DIM, ct::core::ADCGScalar>& x,
	const core::ControlVector<CONTROL_DIM, ct::core::ADCGScalar>& u,
	ct::core::ADCGScalar t)
{
	return evalLocal<ct::core::ADCGScalar>(x, u, t);
}

template <size_t STATE_DIM, size_t CONTROL_DIM, typename SCALAR_EVAL, typename SCALAR>
core::StateVector<STATE_DIM, SCALAR_EVAL> TermQuadMult<STATE_DIM, CONTROL_DIM, SCALAR_EVAL, SCALAR>::stateDerivative(
	const core::StateVector<STATE_DIM, SCALAR_EVAL>& x,
	const core::ControlVector<CONTROL_DIM, SCALAR_EVAL>& u,
	const SCALAR_EVAL& t)
{
	core::StateVector<STATE_DIM, SCALAR_EVAL> xDiff = (x - x_ref_);
	core::ControlVector<CONTROL_DIM, SCALAR_EVAL> uDiff = (u - u_ref_);

	SCALAR_EVAL r = (uDiff.transpose() * R_ * uDiff)(0, 0);

	return (xDiff.transpose() * Q_.transpose() + xDiff.transpose() * Q_) * r;
}

template <size_t STATE_DIM, size_t CONTROL_DIM, typename SCALAR_EVAL, typename SCALAR>
typename TermQuadMult<STATE_DIM, CONTROL_DIM, SCALAR_EVAL, SCALAR>::state_matrix_t
TermQuadMult<STATE_DIM, CONTROL_DIM, SCALAR_EVAL, SCALAR>::stateSecondDerivative(
	const core::StateVector<STATE_DIM, SCALAR_EVAL>& x,
	const core::ControlVector<CONTROL_DIM, SCALAR_EVAL>& u,
	const SCALAR_EVAL& t)
{
	core::ControlVector<CONTROL_DIM, SCALAR_EVAL> uDiff = (u - u_ref_);

	SCALAR_EVAL r = (uDiff.transpose() * R_ * uDiff)(0, 0);

	return (Q_ + Q_.transpose()) * r;
}

template <size_t STATE_DIM, size_t CONTROL_DIM, typename SCALAR_EVAL, typename SCALAR>
core::ControlVector<CONTROL_DIM, SCALAR_EVAL>
TermQuadMult<STATE_DIM, CONTROL_DIM, SCALAR_EVAL, SCALAR>::controlDerivative(
	const core::StateVector<STATE_DIM, SCALAR_EVAL>& x,
	const core::ControlVector<CONTROL_DIM, SCALAR_EVAL>& u,
	const SCALAR_EVAL& t)
{
	core::StateVector<STATE_DIM, SCALAR_EVAL> xDiff = (x - x_ref_);
	core::ControlVector<CONTROL_DIM, SCALAR_EVAL> uDiff = (u - u_ref_);

	SCALAR_EVAL q = (xDiff.transpose() * Q_ * xDiff)(0, 0);

	return (uDiff.transpose() * R_.transpose() + uDiff.transpose() * R_) * q;
}

template <size_t STATE_DIM, size_t CONTROL_DIM, typename SCALAR_EVAL, typename SCALAR>
typename TermQuadMult<STATE_DIM, CONTROL_DIM, SCALAR_EVAL, SCALAR>::control_matrix_t
TermQuadMult<STATE_DIM, CONTROL_DIM, SCALAR_EVAL, SCALAR>::controlSecondDerivative(
	const core::StateVector<STATE_DIM, SCALAR_EVAL>& x,
	const core::ControlVector<CONTROL_DIM, SCALAR_EVAL>& u,
	const SCALAR_EVAL& t)
{
	core::StateVector<STATE_DIM, SCALAR_EVAL> xDiff = (x - x_ref_);

	SCALAR_EVAL q = (xDiff.transpose() * Q_ * xDiff)(0, 0);

	return (R_ + R_.transpose()) * q;
}

template <size_t STATE_DIM, size_t CONTROL_DIM, typename SCALAR_EVAL, typename SCALAR>
typename TermQuadMult<STATE_DIM, CONTROL_DIM, SCALAR_EVAL, SCALAR>::control_state_matrix_t
TermQuadMult<STATE_DIM, CONTROL_DIM, SCALAR_EVAL, SCALAR>::stateControlDerivative(
	const core::StateVector<STATE_DIM, SCALAR_EVAL>& x,
	const core::ControlVector<CONTROL_DIM, SCALAR_EVAL>& u,
	const SCALAR_EVAL& t)
{
	core::StateVector<STATE_DIM, SCALAR_EVAL> xDiff = (x - x_ref_);
	core::ControlVector<CONTROL_DIM, SCALAR_EVAL> uDiff = (u - u_ref_);

	return (uDiff.transpose() * R_.transpose() + uDiff.transpose() * R_).transpose() *
		   (xDiff.transpose() * Q_.transpose() + xDiff.transpose() * Q_);
}

template <size_t STATE_DIM, size_t CONTROL_DIM, typename SCALAR_EVAL, typename SCALAR>
void TermQuadMult<STATE_DIM, CONTROL_DIM, SCALAR_EVAL, SCALAR>::loadConfigFile(const std::string& filename,
	const std::string& termName,
	bool verbose)
{
	loadMatrixCF(filename, "Q", Q_, termName);
	loadMatrixCF(filename, "R", R_, termName);
	loadMatrixCF(filename, "x_des", x_ref_, termName);
	loadMatrixCF(filename, "u_des", u_ref_, termName);
	if (verbose)
	{
		std::cout << "Read Q as Q = \n" << Q_ << std::endl;
		std::cout << "Read R as R = \n" << R_ << std::endl;
		std::cout << "Read x_ref as x_ref = \n" << x_ref_.transpose() << std::endl;
		std::cout << "Read u_ref as u_ref = \n" << u_ref_.transpose() << std::endl;
	}
}

}  // namespace optcon
}  // namespace ct
