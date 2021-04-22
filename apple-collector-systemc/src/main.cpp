#include "systemc.h"
#include "TestBench.h"
#include "AppleCollector.h"
#include <string>

#include "AppleCollector.h"
#include <iostream>

int sc_main(int argc, char *argv[])
{
	// Channel definitions:
    
    //Panel
    sc_fifo<bool> btn_start(1);
    sc_fifo<bool> btn_stop(1);

    //Apple_Sensor
	sc_signal<TestBench::AppleHeight> apple_sensed(0);
    sc_fifo<bool> apple_harvested(1);

    //Direction_Sensor
	sc_fifo<TestBench::Direction> direction(1);
    sc_signal<bool> blocked(0);

    //Tank
    sc_fifo<int> add_fuel(1);

    //Charger
    sc_signal<bool> plugged_in;

    //Lights
    sc_signal<bool> on;
    sc_signal<bool> blink_left;
    sc_signal<bool> blink_right;
    sc_signal<bool> danger_lights;  
    sc_signal<bool> done;
    sc_signal<bool> vacuum_active;
    sc_signal<bool> low_battery;
    sc_signal<bool> low_fuel;

    //modules
    Robot robot("robot");

    // Connect testbench ports to channels:
	TestBench tb("TestBench", argv[1]);
    tb.btn_start(btn_start);
    tb.btn_stop(btn_stop);
    tb.apple_sensed(apple_sensed);
    tb.apple_harvested(apple_harvested);
	tb.direction(direction);
    tb.blocked(blocked);
    tb.add_fuel(add_fuel);
    tb.plugged_in(plugged_in);
    tb.on(on);
    tb.blink_left(blink_left);
    tb.blink_right(blink_right);
    tb.danger_lights(danger_lights);
    tb.done(done);
    tb.vacuum_active(vacuum_active);
    tb.low_battery(low_battery);
    tb.low_fuel(low_fuel);

	// TODO connect your module(s) to channels
    //bind channel between panel and robot:
    robot.btn_start(btn_start);
    robot.btn_stop(btn_stop);

    //bind channel between apple_sensor and robot:
    robot.apple_harvested(apple_harvested);
    robot.apple_sensed(apple_sensed);

    //bind channel between direction_sensor and robot:
    robot.direction(direction);
    robot.blocked(blocked);
    
    //bind channel between battery and charger:
    robot.plugged_in(plugged_in);

    //bind channel between tank and fuel:
    robot.add_fuel(add_fuel);

    //bind channel for lights in robot:
    robot.on(on);
    robot.blink_left(blink_left);
    robot.blink_right(blink_right);
    robot.danger_lights(danger_lights);
    robot.done(done);
    robot.vacuum_active(vacuum_active);
    robot.low_battery(low_battery);
    robot.low_fuel(low_fuel);

	// Run simulation:
	sc_start(13000, SC_SEC); //nicht verändern
	return 0;
}
