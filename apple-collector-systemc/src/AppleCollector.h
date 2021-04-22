#ifndef HA2_ROBOT_H
#define HA2_ROBOT_H

#include "systemc.h"

using namespace std;
SC_MODULE(Robot){

    //local variables:
    int movement_mode;
    bool free_way = true;

    int apple_num;
    bool harvesting;

    int battery_amount;
    int battery_req_vacuum;
    int battery_req_harvest;
    bool charging_allowed;
    bool harvesting_allowed;
    bool harvest_successful;

    int tank_amount;
    int refill_amount;

    bool finished;
    bool stand_by;
    bool robot_is_on;

    bool low_fuel_sent;
    bool sufficient_fuel_sent;

    //ports:
    //for panel
    sc_fifo_in<bool> btn_start{"btn_start"};
    sc_fifo_in<bool> btn_stop{"btn_stop"};

    //apple sensor
    sc_fifo_in<bool> apple_harvested{"apple_harvested"};
    sc_in<TestBench::AppleHeight> apple_sensed{"apple_sensed"};

    //for direction sensor
    sc_fifo_in<TestBench::Direction> direction{"direction"};
    sc_in<bool> blocked{"blocked"};

    //for Battery:
    sc_in<bool> plugged_in{"plugged_in"};

    //for Tank:
    sc_fifo_in<int> add_fuel{"add_fuel"};

    //ports for light signals:
    sc_out<bool> on{"on"};//
    sc_out<bool> vacuum_active{"vacuum_active"};
    sc_out<bool> done{"done"};
    sc_out<bool> blink_left{"blink_left"};
    sc_out<bool> blink_right{"blink_right"};
    sc_out<bool> danger_lights{"danger_lights"};
    sc_out<bool> low_battery{"low_battery"};
    sc_out<bool> low_fuel{"low_fuel"};

    //local events
    sc_event waiter_6_sec;
    sc_event begin_harvesting;
    sc_event event_is_there;
    sc_event harvesting_process_ended;

    sc_event begin_to_charge;

    sc_event battery_changed;
    sc_event tank_empty;
    //sc_event refill_event;

    sc_event hundred_apples_harvested; // 100 apples harvested = done (need additional variable as 'on' can only have one driver)

    SC_CTOR(Robot){
        movement_mode = -1;
        apple_num = 0;
        harvesting = false;
        battery_amount = 0;
        tank_amount = 0;
        refill_amount = 0;
        charging_allowed = false;
        finished = false;
        battery_req_vacuum = 0;
        battery_req_harvest = 0;
        harvest_successful = false;
        stand_by = true;
        low_fuel_sent = false;
        sufficient_fuel_sent = false;
        harvesting_allowed = false;
        robot_is_on = false;

        // Initialize out ports
        on.initialize(false);
        vacuum_active.initialize(false);
        done.initialize(false);
        blink_left.initialize(false);
        blink_right.initialize(false);
        danger_lights.initialize(false);
        low_battery.initialize(true);
        low_fuel.initialize(true);

        SC_THREAD(switcher);

        //SC_THREAD(switch_off);

        SC_THREAD(move);
        SC_THREAD(way_validate);
        sensitive << blocked;

        SC_THREAD(farm);
        sensitive << apple_sensed;
        sensitive << begin_harvesting;
        SC_THREAD(harvester);
        sensitive << begin_harvesting;

        SC_THREAD(monitor_battery);
        sensitive << plugged_in.neg() << battery_changed;

        SC_THREAD(charging);
        sensitive << plugged_in.pos();
        SC_THREAD(charging_now);
        sensitive << begin_to_charge;

        SC_THREAD(monitor_tank); 
        sensitive << on.pos();

        SC_THREAD(fill);

    }

    void switcher() {
        cout << "Entered Robot.switch_on!" << endl;
        while(true) {

            bool cable = btn_start.read();
            if (cable){
                if(battery_amount >= 10 && tank_amount >= 50 && movement_mode == -1){
                    cout << "Switched on detected!" << endl;
                    on->write(true);
                    robot_is_on = true;
                }
                    stand_by = false;
            }

            if(battery_amount >= 10 && tank_amount >= 50 && robot_is_on){
                cout << "Entered Robot.switch_off!" << endl;
                while(true) {

                    wait(btn_stop.data_written_event() | hundred_apples_harvested | tank_empty);

                    if ((finished && (!harvesting_allowed || apple_num == 100))) {
                        cout << "Switched off detected!" << endl;
                        stand_by = true;
                        on->write(false);
                        robot_is_on = false;
                    } else {
                        bool cable = btn_stop.read();
                        if (cable){
                            cout << "Switched off detected!" << endl;
                            stand_by = true;
                            if(harvesting_allowed){
                                cout << "waiting of harvesting" << endl;
                                wait(harvesting_process_ended);//when btn_stop is true and robot harvests, then wait until harvesting is finished.
                                cout << "end waiting of harvesting" << endl; 
                            }
                            on->write(false);
                            robot_is_on = false;
                        }
                    }
                    if(!robot_is_on)
                        break;
                }
            }

            if (finished) {
                stand_by = true;
                on->write(false);
                robot_is_on = false;
            }

        }
    }

    void move() {
        while(true) {
            int direction_value = direction.read();
            blink_right.write(false);
            blink_left.write(false);
            if(robot_is_on){
            movement_mode = direction_value;
            ///////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////////                 Movement                         ///////////////
            ///////////////////////////////////////////////////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////////                 drive                         //////////////////
            if((direction_value == TestBench::go_straight || movement_mode == TestBench::go_straight) && free_way && !charging_allowed){
                cout << "roboter in drive mode" << endl;
            }
            ///////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////////                 left                         ///////////////////
            else if((direction_value == TestBench::turn_left || movement_mode == TestBench::turn_left) && free_way && !harvesting_allowed && !charging_allowed){
                cout << "roboter in left mode" << endl;
                blink_left.write(true);
            }
            ///////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////////                 right                        ///////////////////
            else if((direction_value == TestBench::turn_right || movement_mode == TestBench::turn_right) && free_way  && !harvesting_allowed && !charging_allowed){
                cout << "roboter in right mode" << endl;
                blink_right.write(true);
            }
            ///////////////////////////////////////////////////////////////////////////////////////////////
            //////////////////////////////                 stand                         //////////////////
            else {
                cout << "roboter in stand mode" << endl;
                movement_mode = -1;
            }
            }
        }
    }
    void way_validate(){
        wait();
        free_way = !blocked.read();
        if(robot_is_on){
        cout << "roboter in blocked mode" << endl;
            if(!free_way)
                danger_lights.write(true);
                movement_mode = -1;
        }
    }

    void farm() {

        while(true) {

            wait();

            if(robot_is_on){
            cout << "change in apple_sensed detected!" << endl;

            int apple_level = apple_sensed.read();

            int arm_aus_einfahren = 0;          

            if(apple_level != TestBench::no_apple){
                if(apple_level == TestBench::apple_low && movement_mode == TestBench::go_straight){
                    movement_mode = -1;
                    cout << "apple is near" << endl;
                    arm_aus_einfahren = 1;
                    harvesting_allowed = battery_amount > 25;
                    if(harvesting_allowed)
                        battery_req_vacuum = 1;
                    else cout << "insufficient battery, can't harvest low apple!" << endl;
                }
                else if(apple_level == TestBench::apple_high && movement_mode == TestBench::go_straight){
                    movement_mode = -1;
                    cout << "apple is far" << endl;
                    arm_aus_einfahren = 5;
                    harvesting_allowed = battery_amount > 50;
                    if(harvesting_allowed)
                        battery_req_vacuum = 6;
                    else cout << "insufficient battery, can't harvest high apple!" << endl;
                }
                else if(apple_level == TestBench::apple_very_high && movement_mode == TestBench::go_straight){
                    movement_mode = -1;
                    cout << "apple is very far" << endl;
                    arm_aus_einfahren = 8;
                    harvesting_allowed = battery_amount > 50;
                    if(harvesting_allowed)
                        battery_req_vacuum = 11;
                    else cout << "insufficient battery, can't harvest very high apple!" << endl;
                }

                if(harvesting_allowed){
                    movement_mode = -1;
                    wait(arm_aus_einfahren, SC_SEC);
                    battery_amount -= battery_req_vacuum;
                    notify(battery_changed);
                    vacuum_active.write(true);
                    cout << "vacuum is on" << endl;
                    notify(begin_harvesting);
                    cout << "begin harvesting!" << endl;
                    wait(event_is_there);
                    cout << "harvest finished!" << endl;
                    vacuum_active.write(false);
                    notify(harvesting_process_ended);
                    harvest_successful = false;
                    battery_amount -= battery_req_harvest;
                    notify(battery_changed);
                    wait(arm_aus_einfahren, SC_SEC);
                    battery_amount -= battery_req_vacuum;
                    notify(battery_changed);
                    cout << "vacuum is off" << endl;
                    notify(battery_changed);
                }
            }
            }
        }
    }

    void harvester(){

        while(true) {

            wait();

            // First scenario: harvesting took less than 2 seconds

            //wait(apple_harvested.data_written_event() | timer);

            wait(1999, SC_MS, apple_harvested.data_written_event());

            if (apple_harvested.num_available() > 0 && apple_harvested.read()) {
                cout << "Harvest completed in < 2 seconds." << endl;
                battery_req_harvest = 5;
                harvest_successful = true;
                notify(event_is_there);
                apple_num++;
                cout << "a new apple  harvested" << endl;
                if (apple_num == 100) {
                    done.write(true);
                    finished = true;
                    notify(hundred_apples_harvested);
                }
            } else {

                // Second scenario: harvesting took more than 2 but less than 4 seconds

                //wait(apple_harvested.data_written_event() | timer);
                wait(2000, SC_MS, apple_harvested.data_written_event());

                if (apple_harvested.num_available() > 0 && apple_harvested.read()) {
                    cout << "Harvest completed in at least 2, but less than 4 seconds." << endl;
                    battery_req_harvest = 10;
                    harvest_successful = true;
                    notify(event_is_there);
                    apple_num++;
                    cout << "a new apple  harvested" << endl;
                    if (apple_num == 100) {
                        done.write(true);
                        finished = true;
                        notify(hundred_apples_harvested);
                    }
                } else {

                    // Third scenario: harvesting took more than 4 but less than 6 seconds

                    //wait(apple_harvested.data_written_event() | timer);
                    wait(2000, SC_MS, apple_harvested.data_written_event());

                    if (apple_harvested.num_available() > 0 && apple_harvested.read()) {
                        cout << "Harvest completed in at least 4, but less than 6 seconds." << endl;
                        battery_req_harvest = 15;
                        harvest_successful = true;
                        notify(event_is_there);
                        apple_num++;
                        cout << "a new apple  harvested" << endl;
                        if (apple_num == 100) {
                            done.write(true);
                            finished = true;
                            notify(hundred_apples_harvested);
                        }
                    } else {

                        // Fourth scenario: no apple harvested after 6 seconds

                        wait(1, SC_MS);
                        cout << "6 seconds have elapsed, harvest unsuccessful." << endl;
                        battery_req_harvest = 20;
                        notify(event_is_there);
                        cout << "No apple could be harvested" << endl;
                    }
                }
            }
        }
    }

    void monitor_battery() {
        while(true) {
            wait();
            cout << "checking battery status." << endl;
            if (! plugged_in.read()) {
                charging_allowed = false;
                cout << "Setting charging allowed to false!" << endl;
            }
            cout << "there are: " << battery_amount << "% in Battery" << endl;
            if(battery_amount < 30) {
                low_battery.write(true);
                cout << "battery is low." << endl;
            }
            else {
                low_battery.write(false);
                cout << "battery is not low." << endl;
            }
            
        }
    }

    void charging(){

        while(true) {
            wait();

            cout << "Entered battery.charging!" << endl;

            cout << "Plugged in!" << endl;
            charging_allowed = true;
            notify(begin_to_charge);
  
            //cout << "i am waiting until plugged_in changed." << endl;
        }
    }
    void charging_now(){
        while(true) {
            wait();
            cout << "begin to charge." << endl;
            while(charging_allowed){
                wait(3, SC_SEC);
                if (charging_allowed) {
                    if(battery_amount <= 95)
                        battery_amount += 5;
                    else
                        battery_amount = 100;
                }
                //cout << "there are: " << battery_amount << "% in Battery" << endl;
            }
            cout << "Charging finished!" << endl;
            cout << "there are: " << battery_amount << "% in Battery" << endl;
        }
    }

    void fill() {
        cout << "Entered Tank.fill!" << endl;
        while (true){
            refill_amount = add_fuel.read();

            //notify(refill_event);

            if (tank_amount == 500 || tank_amount + refill_amount > 500) {
                tank_amount = 500;
                
            } else {
                tank_amount += refill_amount;
                cout << "Fuel added!" << endl;
            }

            if (tank_amount >= 50) 
                low_fuel_sent = false;

            cout << "tank amount is " + tank_amount << endl;
            cout << "Waiting for add_fuel!" << endl;
        }
    }
    
    void monitor_tank() {

        while(true) {

            if (tank_amount < 10 && robot_is_on) {
                cout << "Running out of fuel, switching off!" << endl;
                finished = true;
                notify(tank_empty);
                
            } else if (tank_amount >= 50 && !sufficient_fuel_sent) {
                low_fuel.write(false);
                cout << "Setting low_fuel to false!" << endl;
                sufficient_fuel_sent = true;

            } else  if (tank_amount < 50 && !low_fuel_sent) {
                low_fuel.write(true);
                cout << "Setting low_fuel to true!" << endl;
                cout << "Tank contents: " << tank_amount << "ml" << endl;
                low_fuel_sent = true;
                sufficient_fuel_sent = false;
            }

            wait(3, SC_SEC);

            if (!stand_by && movement_mode == -1 && robot_is_on) { // robot in stand mode
                tank_amount -= 1;
                cout << "Consuming fuel while standing!" << endl;
                cout << "Tank contents: " << tank_amount << "ml" << endl;
            } else if (! stand_by && movement_mode != -1 && robot_is_on) {
                tank_amount -= 2;   // robot driving
                battery_amount += 1;
                cout << "Consuming fuel while driving, generating power!" << endl;
                cout << "Tank contents: " << tank_amount << "ml" << endl;
            }
        }
    }
};
#endif //HA2_ROBOT_H
