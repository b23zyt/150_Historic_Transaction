#include <cassert>
#include <iomanip>
#include <iostream>

#include "project4.hpp"
#include "History.hpp"
#include "Transaction.hpp"

////////////////////////////////////////////////////////////////////////////////
// Definitions for Transaction class
////////////////////////////////////////////////////////////////////////////////
//
//

// Constructor
// TODO
Transaction::Transaction(std::string ticker_symbol, unsigned int day_date, unsigned int month_date, unsigned year_date, 
  bool buy_sell_trans, unsigned int number_shares, double trans_amount ) : 
  symbol{ticker_symbol}, day{day_date}, month{month_date}, year{year_date}, shares{number_shares}, amount{trans_amount},
  trans_id{assigned_trans_id}, acb{0.0}, acb_per_share{0.0}, share_balance{0}, cgl{0.0} {
    
    trans_type = (buy_sell_trans) ? "Buy" : "Sell";
    assigned_trans_id++;
    p_next = nullptr;
}

// Destructor
// TODO
Transaction::~Transaction(){
}

// Overloaded < operator.
// TODO 不犯毛病
bool Transaction::operator<( Transaction const &other ){
  if (year < other.year){
    return true;
  }
  else if (year == other.year){
    if (month < other.month){
      return true;
    }
    else if(month == other.month){
      if (day < other.day){
        return true;
      }
      else if (day == other.day){
        return trans_id < other.trans_id;
      }
    }
  }
  return false;
}

// GIVEN
// Member functions to get values.
//
std::string Transaction::get_symbol() const { return symbol; }
unsigned int Transaction::get_day() const { return day; }
unsigned int Transaction::get_month() const { return month; }
unsigned int Transaction::get_year() const { return year; }
unsigned int Transaction::get_shares() const { return shares; }
double Transaction::get_amount() const { return amount; }
double Transaction::get_acb() const { return acb; }
double Transaction::get_acb_per_share() const { return acb_per_share; }
unsigned int Transaction::get_share_balance() const { return share_balance; }
double Transaction::get_cgl() const { return cgl; }
bool Transaction::get_trans_type() const { return (trans_type == "Buy") ? true: false ; }
unsigned int Transaction::get_trans_id() const { return trans_id; }
Transaction *Transaction::get_next() { return p_next; }

// GIVEN
// Member functions to set values.
//
void Transaction::set_acb( double acb_value ) { acb = acb_value; }
void Transaction::set_acb_per_share( double acb_share_value ) { acb_per_share = acb_share_value; }
void Transaction::set_share_balance( unsigned int bal ) { share_balance = bal ; }
void Transaction::set_cgl( double value ) { cgl = value; }
void Transaction::set_next( Transaction *p_new_next ) { p_next = p_new_next; }

// GIVEN
// Print the transaction.
//
void Transaction::print() {
  std::cout << std::fixed << std::setprecision(2);
  std::cout << std::setw(4) << get_trans_id() << " "
    << std::setw(4) << get_symbol() << " "
    << std::setw(4) << get_day() << " "
    << std::setw(4) << get_month() << " "
    << std::setw(4) << get_year() << " ";


  if ( get_trans_type() ) {
    std::cout << "  Buy  ";
  } else { std::cout << "  Sell "; }

  std::cout << std::setw(4) << get_shares() << " "
    << std::setw(10) << get_amount() << " "
    << std::setw(10) << get_acb() << " " << std::setw(4) << get_share_balance() << " "
    << std::setw(10) << std::setprecision(3) << get_acb_per_share() << " "
    << std::setw(10) << std::setprecision(3) << get_cgl()
    << std::endl;
}


////////////////////////////////////////////////////////////////////////////////
// Definitions for the History class
////////////////////////////////////////////////////////////////////////////////
//
//

// Constructor
History::History() : p_head{nullptr}{
}

// Destructor
History::~History(){
  while(p_head != nullptr){
    Transaction* temp = p_head;
    p_head = p_head->get_next();
    delete temp;
  }
  p_head = nullptr;
}

// insert(...): Insert transaction into linked list.
void History::insert(Transaction *p_new_trans){
  if (p_head == nullptr){
    p_head = p_new_trans;
  }
  else{
    Transaction *current{p_head};
    while(current->get_next() != nullptr){ 
      current = current->get_next();  //iterate through the list until it reaches the last transaction
    }
    current->set_next(p_new_trans); //reach the last transaction, insert p_new_trans after the last transaction (end of the linked list)
  }
}

// read_history(...): Read the transaction history from file.
// TODO
void History::read_history(){
  ece150::open_file();
  while(ece150::next_trans_entry()){ //true
    std::string symbol{ece150::get_trans_symbol()};
    unsigned int day{ece150::get_trans_day()};
    unsigned int month{ece150::get_trans_month()};
    unsigned int year{ece150::get_trans_year()};
    bool buy_or_sell{ece150::get_trans_type()};
    unsigned int shares{ece150::get_trans_shares()};
    double amount{ece150::get_trans_amount()};
    Transaction* new_transactions{new Transaction(symbol, day, month, year, buy_or_sell, shares, amount)};
    insert(new_transactions);
  }
  ece150::close_file();
}

// print() Print the transaction history.
void History::print(){
 Transaction* current{p_head};
 std::cout << "========== BEGIN TRANSACTION HISTORY ==========" <<std::endl;
  while(current != nullptr){
    current->print();
    current = current->get_next();
  }
  std::cout << "========== END TRANSACTION HISTORY ==========" << std::endl;
}

// sort_by_date(): Sort the linked list by trade date.
// TODO
void History::sort_by_date(){
  Transaction* current{p_head};
  Transaction* big_pre_current{current}; //used to keep track of the node before current
  //std::cout << current; (debugging)
  
  while(current->get_next() != nullptr){
    Transaction* min_transaction{current};
    Transaction* next_transaction{current->get_next()};
    Transaction* pre_current{nullptr}; 
    Transaction* pre_min{nullptr};
    bool changed{false}; //used to indicate if changes has occured

    while (next_transaction != nullptr){
      if (*next_transaction < *min_transaction){
        min_transaction = next_transaction;
        pre_min = pre_current; //update the node before min_transaction
        changed = true;
      }

      pre_current = next_transaction;
      next_transaction = next_transaction->get_next();
    }
      
    //std::cout << std::endl<<current->get_trans_id() << ":::"<< p_head->get_trans_id()<<std::endl; (debugging)
    
    if (changed){
    //std::cout << "test"; (debugging)
      // cond1: min_transaction needs to be inserted somewhere in the middle of the list (not the first or end)
      if((pre_min != nullptr) && (p_head != current)){
        pre_min->set_next(min_transaction->get_next()); //update p_min before min_transaction
        min_transaction->set_next(current); //update the next pointer of min_transaction to the node points to current.
        big_pre_current->set_next(min_transaction); //update the next pointer of current to the node points to next_transaction
        big_pre_current = min_transaction; //update big_pre_current to min_transaction to make sure that it starts from the min_transaction for the next iteration
      }
      //cond2: min_transaction needs to be inserted at the beginning of the list, during subsequent iterations
      else if((pre_min != nullptr) && (p_head == current)){ 
        pre_min->set_next(min_transaction->get_next()); //update p_min before min_transaction
        min_transaction->set_next(current);  //update the next pointer of min_transaction to the node points to current.
        p_head = min_transaction; //Updates the head of the list to be min_transaction
        big_pre_current = min_transaction; //update big_pre_current to min_transaction to make sure that it starts from the min_transaction for the next iteration
      }

      //cond3: min_transaction needs to be inserted at the beginning of the list, during the first iteration
      else if((pre_min == nullptr) && (p_head != current)){
        pre_min = current; //since there's no node before current, pre_min become the current.
        pre_min->set_next(min_transaction->get_next()); //update p_min before min_transaction
        min_transaction->set_next(current); //update the next pointer of min_transaction to the node points to current.
        big_pre_current->set_next(min_transaction); //update the next pointer of current to point to next_transaction
        big_pre_current = min_transaction;
      }
      
      //cond4: (pre_min == nullptr) && (p_head == current) //min_transaction needs to be inserted as the first node, first iteration
      else{ 
        pre_min = current; //set pre_min to the beginning of the link
        pre_min->set_next(min_transaction->get_next()); //update p_min before min_transaction
        min_transaction->set_next(current); //insert min before current
        big_pre_current = min_transaction;
      }
    }

    //if no changes occurred
    else{ 
      big_pre_current = current;  //updates big_pre_current
      current = current->get_next();
    }

    //resets pre_min and changed before next the iteration
    pre_min = nullptr;
    changed = false;
  }
}

// update_acb_cgl(): Updates the ACB and CGL values.
// TODO
void History::update_acb_cgl(){
  Transaction* current{p_head};
  double acb{0.0};
  unsigned int share_balance{0};
  double prev_acb_per{0.0};

  while (current != nullptr){
    if (current->get_trans_type()){
      acb += current->get_amount();
      share_balance += current->get_shares();
    }

    else{
      acb -= current->get_shares()*prev_acb_per;
      share_balance -= current->get_shares();

      double cgl{current->get_amount() - current->get_shares()*prev_acb_per};
      current->set_cgl(cgl);
    }

    current->set_acb(acb);
    current->set_share_balance(share_balance);
    current->set_acb_per_share(acb/share_balance);
    
    prev_acb_per = acb/share_balance;
    current = current->get_next();
  }
}

// compute_cgl(): )Compute the ACB, and CGL.
// TODO
double History::compute_cgl(unsigned int year){
  Transaction* current{p_head};
  double cgl{0.0};
  while(current != nullptr){
    if (current->get_year() == year){
      cgl += current->get_cgl();
    }
    current = current->get_next();
  }
  return cgl; 
}


// GIVEN
// get_p_head(): Full access to the linked list.
//
Transaction *History::get_p_head() { 
  return p_head; 
}
