#include "TrinomialPrice.h"
#include <math.h>

// Set up the Trinomial Pricing Parameters
TrinomialPrice::TrinomialPrice(Option* pOpt, int nSteps)
	: option(pOpt), steps(nSteps), lattice(new Lattice()) { 
	lattice->root = new Node(option->spotPrice);
	lattice->root->level = 0;

	// Calculate up and down factors
	dt = option->maturity / steps;
	u = exp(lambda * option->vol * sqrt(dt));
	d = 1 / u;

	buildLattice(lattice->root);

	// Risk-neutral probabilities
	double m = exp(option->rate * dt);
	double v = m * m * (exp(option->vol * option->vol * dt) - 1);
	pu = ((v + m * m - m) * u - (m - 1)) / ((u - 1) * (u * u - 1));
	pd = ((v + m * m - m) * d - (m - 1)) / ((d - 1) * (d * d - 1));
	ph = 1 - pu - pd;


	// Discount factor
	discount = exp(-option->rate * dt);
}

// Build the Trinomial Lattice recursively
void TrinomialPrice::buildLattice(Node* node) {
	// Base case: if node's level reaches the total steps, stop recursion
	if (node == nullptr || node->level >= steps) return;

	// Calculate new stock prices for up, horizontal, and down nodes
	double upPrice = node->data.sharePrice * u;
	double horizontalPrice = node->data.sharePrice;
	double downPrice = node->data.sharePrice * d;

	// Create and link the new nodes
	node->up = new Node(upPrice);
	node->up->level = node->level + 1;

	node->horizontal = new Node(horizontalPrice);
	node->horizontal->level = node->level + 1;

	node->down = new Node(downPrice);
	node->down->level = node->level + 1;

	// Recur for the new nodes
	buildLattice(node->up);
	buildLattice(node->horizontal);
	buildLattice(node->down);
}

// Calculate the option value using recursion
void TrinomialPrice::backwardEval(Node* node) {
	// Base case: if the node has no children (leaf node), assign option price based on the payoff
	if (node->level == steps) {
		node->data.optionPrice = MAX(0, OPTION_MULTIPLIER(option->c_p) *
			(node->data.sharePrice - option->strikePrice));
		return;
	}

	// Recursively evaluate child nodes
	backwardEval(node->up);
	backwardEval(node->horizontal);
	backwardEval(node->down);

	// Calculate the expected option value at this node
	double expectedValue = pu * node->up->data.optionPrice +
		ph * node->horizontal->data.optionPrice +
		pd * node->down->data.optionPrice;
	double intrinsicValue = OPTION_MULTIPLIER(option->c_p) *
		(node->data.sharePrice - option->strikePrice);

	// Determine option price based on type (European or American)
	if (option->e_a == American) {
		node->data.optionPrice = MAX(intrinsicValue, discount * expectedValue);
	}
	else {  // European
		node->data.optionPrice = discount * expectedValue;
	}
}