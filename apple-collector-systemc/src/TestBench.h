#ifndef TESTBENCH_H_
#define TESTBENCH_H_

#include "systemc.h"
#include <string>
#include <map>

using namespace std;

SC_MODULE(TestBench)
{
    // ------------------- Outputs -------------------
    //Panel
    sc_fifo_out<bool> btn_start;
    sc_fifo_out<bool> btn_stop;

    //Apple_Sensor
    enum AppleHeight
    {
        no_apple = 0,
        apple_low = 1,
        apple_high = 2,
        apple_very_high = 3
    };
    sc_out<AppleHeight> apple_sensed;
    sc_fifo_out<bool> apple_harvested;

    //Direction_Sensor
    enum Direction
    {
        go_straight = 0,
        turn_left = 1,
        turn_right = 2
    };
    sc_fifo_out<Direction> direction;
    sc_out<bool> blocked;

    //Tank
    sc_fifo_out<int> add_fuel;

    //Charger
    sc_out<bool> plugged_in;
    

    // ------------------- Inputs -------------------
    sc_in<bool> on;
    sc_in<bool> blink_left;
    sc_in<bool> blink_right;
    sc_in<bool> danger_lights;
    sc_in<bool> done;
    sc_in<bool> vacuum_active;
    sc_in<bool> low_battery;
    sc_in<bool> low_fuel; 


    // ------------------- Internal -------------------
    SC_HAS_PROCESS(TestBench);
    TestBench(sc_module_name name, string test_case_name);

    typedef void (TestBench::*FnPtr)(void);
    FnPtr my_test_case;
    void thread_loop();
    void timeout();


private:
    void testTurnOn();
    void testTurnOff();
    void testTurnOnWithoutBattery();
    void testTurnLeft();
    void testTurnRight();
    void testHaltOnGoStraight();
    void testLowAppleTiming();
    void testHighAppleTiming();
    void testVeryHighAppleTiming();
    void testLowAppleNotEnoughBattery();
    void testHighAppleNotEnoughBattery();
    void testVeryHighAppleNotEnoughBattery();
    void testAppleLowBatteryConsumption();
    void testAppleHighBatteryConsumption();
    void testAppleVeryHighBatteryConsumption();
    void testAppleLowBatteryConsumptionOnWayDown();
    void testAppleHighBatteryConsumptionOnWayDown();
    void testAppleVeryHighBatteryConsumptionOnWayDown();
    void testAppleHarvestingTimout();
    void testAppleHarvestingEasy();
    void testAppleHarvestingHard();
    void testAppleHarvestingVeryHard();
    void testAppleHarvestingEasyBatteryConsumption();
    void testAppleHarvestingHardBatteryConsumption();
    void testAppleHarvestingVeryHardBatteryConsumption();
    void testAppleHarvestingTimeoutBatteryConsumption();
    void testFuelConsumptionWhileStanding();
    void testStartBelow50ml();
    void testTankEmpty100ml();
    void testTankEmpty500ml();
    void testTankLowFuel();
    void testCollect100Apples();
    void testInitialState();
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void testDirectionIgnoredWhileHarvesting();
    void testSwitchOnWithEmptyTank();
    void testAutomaticSwitchOffWhenTankEmpty();
    void testOffButtonWhileHarvesting();
    void testChargingTransitionOffOn();
    void testChargingTransitionOnOff();
    void testNoReturnToMotionHistoryWithCable();
    void testNoReturnToMotionFromHarvestingWithCable();
    void testHistoryFunctionality();
    void testHaltOnTurnLeft();
    void testHaltOnTurnRight();
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
};

#endif