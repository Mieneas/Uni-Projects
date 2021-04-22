#ifndef TESTHELPER_H_
#define TESTHELPER_H_

#include "systemc.h"

using namespace std;

#define SEQ_WAIT 100 // Time to wait between subsequent raise statements

void print(string s);

template <typename Type1>
void send_signal(sc_out<Type1> *port, string port_name, Type1 value)
{
    print("Setting '" + port_name + "'='" + std::to_string(value) + "'");
    port->write(value);
    wait(SEQ_WAIT, SC_MS);
}

template <typename Type2>
void assert_now(sc_in<Type2> *port, string port_name, Type2 expected, string message = "")
{
    // Check if signal value matches currently
    if (port->read() != expected)
    {
        print("Assertion failed: '" + port_name + "' is not '" + std::to_string(expected) + "'");
        exit(1);
    }
}

template <typename Type3>
void send_signal_fifo(sc_fifo_out<Type3> *port, string port_name, Type3 value)
{
    print("Setting '" + port_name + "'='" + std::to_string(value) + "'");
    port->write(value);
    wait(SEQ_WAIT, SC_MS);
}

template <typename Type4>
void assert_now(sc_fifo_in<Type4> *port, string port_name, Type4 expected, string message = "")
{
    // Check if signal value matches currently
    if (port->read() != expected)
    {
        print("Assertion failed: '" + port_name + "' is not '" + std::to_string(expected) + "'");
        exit(1);
    }
}


#endif