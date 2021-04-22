#include "TestBench.h"
#include "TestHelper.h"
#include <map>
#include <string>

/*
 * Das sind die Testfälle für das Basisverhalten vom Roboter.
 * Sie sind den Basistests der ersten Hausaufgabe sehr ähnlich.
 * Wenn euer Modell alle hier aufgeführten Testfälle besteht, bekommt ihr mindestens 8 Punkte.
 * Wenn euer Modell nicht alle hier aufgehführten Testfälle besteht, bekommt ihr 0 Punkte.
 * 
 * Um die Tests auzuführen, müsst ihr folgende Befehle im Terminal eingeben:
 * 
 * cmake CMakeLists.txt     //erzeugt und konfiguriert Makefile
 * make                     //Führt Makefile aus und kompiliert euer Projekt
 * ctest                    //Führt euer Projekt mit den Tests aus
 * 
 * Wenn ihr nur einen einzigen Test ausführen wollt, kompiliert euer Projekt und gebt folgenden Befehl ein:
 * 
 * ./ha2 testName
 * 
 * ./ha2 testTurnOn         //Führt testTurnOn aus
*/

bool finished = false;

TestBench::TestBench(sc_module_name name, string test_case_name) : sc_module(name)
{
    // Init signals:
    apple_sensed.initialize(TestBench::no_apple);
    blocked.initialize(false);
    plugged_in.initialize(false);

    // Init test list:
    map<std::string, FnPtr> test_cases;
    
    test_cases["testTurnOn"] = &TestBench::testTurnOn;
    test_cases["testTurnOff"] = &TestBench::testTurnOff;
    test_cases["testTurnOnWithoutBattery"] = &TestBench::testTurnOnWithoutBattery;
    test_cases["testTurnLeft"] = &TestBench::testTurnLeft;
    test_cases["testTurnRight"] = &TestBench::testTurnRight;
    test_cases["testHaltOnGoStraight"] = &TestBench::testHaltOnGoStraight;
    test_cases["testLowAppleTiming"] = &TestBench::testLowAppleTiming;
    test_cases["testHighAppleTiming"] = &TestBench::testHighAppleTiming;
    test_cases["testVeryHighAppleTiming"] = &TestBench::testVeryHighAppleTiming;
    test_cases["testLowAppleNotEnoughBattery"] = &TestBench::testLowAppleNotEnoughBattery;
    test_cases["testHighAppleNotEnoughBattery"] = &TestBench::testHighAppleNotEnoughBattery;
    test_cases["testVeryHighAppleNotEnoughBattery"] = &TestBench::testVeryHighAppleNotEnoughBattery;
    test_cases["testAppleLowBatteryConsumption"] = &TestBench::testAppleLowBatteryConsumption;
    test_cases["testAppleHighBatteryConsumption"] = &TestBench::testAppleHighBatteryConsumption;
    test_cases["testAppleVeryHighBatteryConsumption"] = &TestBench::testAppleVeryHighBatteryConsumption;
    test_cases["testAppleLowBatteryConsumptionOnWayDown"] = &TestBench::testAppleLowBatteryConsumptionOnWayDown;
    test_cases["testAppleHighBatteryConsumptionOnWayDown"] = &TestBench::testAppleHighBatteryConsumptionOnWayDown;
    test_cases["testAppleVeryHighBatteryConsumptionOnWayDown"] = &TestBench::testAppleVeryHighBatteryConsumptionOnWayDown;
    test_cases["testAppleHarvestingTimout"] = &TestBench::testAppleHarvestingTimout;
    test_cases["testAppleHarvestingEasy"] = &TestBench::testAppleHarvestingEasy;
    test_cases["testAppleHarvestingHard"] = &TestBench::testAppleHarvestingHard;
    test_cases["testAppleHarvestingVeryHard"] = &TestBench::testAppleHarvestingVeryHard;
    test_cases["testAppleHarvestingEasyBatteryConsumption"] = &TestBench::testAppleHarvestingEasyBatteryConsumption;
    test_cases["testAppleHarvestingHardBatteryConsumption"] = &TestBench::testAppleHarvestingHardBatteryConsumption;
    test_cases["testAppleHarvestingVeryHardBatteryConsumption"] = &TestBench::testAppleHarvestingVeryHardBatteryConsumption;
    test_cases["testAppleHarvestingTimeoutBatteryConsumption"] = &TestBench::testAppleHarvestingTimeoutBatteryConsumption;
    test_cases["testFuelConsumptionWhileStanding"] = &TestBench::testFuelConsumptionWhileStanding;
    test_cases["testStartBelow50ml"] = &TestBench::testStartBelow50ml;
    test_cases["testTankEmpty100ml"] = &TestBench::testTankEmpty100ml;
    test_cases["testTankEmpty500ml"] = &TestBench::testTankEmpty500ml;
    test_cases["testTankLowFuel"] = &TestBench::testTankLowFuel;
    test_cases["testCollect100Apples"] = &TestBench::testCollect100Apples;
    test_cases["testInitialState"] = &TestBench::testInitialState;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    test_cases["testDirectionIgnoredWhileHarvesting"] =  &TestBench::testDirectionIgnoredWhileHarvesting;
    test_cases["testSwitchOnWithEmptyTank"] =  &TestBench::testSwitchOnWithEmptyTank;
    test_cases["testAutomaticSwitchOffWhenTankEmpty"] =  &TestBench::testAutomaticSwitchOffWhenTankEmpty;
    test_cases["testOffButtonWhileHarvesting"] =  &TestBench::testOffButtonWhileHarvesting;
    test_cases["testChargingTransitionOffOn"] =  &TestBench::testChargingTransitionOffOn;
    test_cases["testChargingTransitionOnOff"] =  &TestBench::testChargingTransitionOnOff;
    test_cases["testNoReturnToMotionHistoryWithCable"] =  &TestBench::testNoReturnToMotionHistoryWithCable;
    test_cases["testNoReturnToMotionFromHarvestingWithCable"] =  &TestBench::testNoReturnToMotionFromHarvestingWithCable;
    test_cases["testHistoryFunctionality"] =  &TestBench::testHistoryFunctionality;
    test_cases["testHaltOnTurnLeft"] =  &TestBench::testHaltOnTurnLeft;
    test_cases["testHaltOnTurnRight"] =  &TestBench::testHaltOnTurnRight;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Select test function:
    if (test_cases.count(test_case_name) == 0)
    {
        cout << "Unknown test function '" << test_case_name << "'" << endl;
        exit(2);
    }
    my_test_case = test_cases[test_case_name];
    SC_THREAD(thread_loop);
    SC_THREAD(timeout);
}

void TestBench::thread_loop()
{
    finished = false;
    (this->*my_test_case)();
    finished = true;
}

void TestBench::timeout()
{
    wait(12900, SC_SEC);
    if (not finished)
    {
        cout << "Test failed: Timeout" << endl;
        exit(1);
    }
}

// Test cases

//test1
	/* Wir testen, ob wir den Roboter über das Bedienpanel einschalten können.
	 * -Der Bauer kann den Roboter jederzeit über ein Eingabepanel ein-und ausschalten (btn_start, btn_stop):
	 * -Wenn der Roboter angeschaltet wird, sendet er das Signal (on).
	 */
void TestBench::testTurnOn()
{
    send_signal_fifo(&add_fuel, "add_fuel", 100);                           //100ml Treibstoff auffüllen
    send_signal(&plugged_in, "plugged_in", true);                           //Ladekabel ansschließen
    wait(60, SC_SEC);                                                       //für 60s aufladen; Akku = 100%
    send_signal(&plugged_in, "plugged_in", false);                          //Ladekabel trennen
    send_signal_fifo(&btn_start, "btn_start", true);                        //Roboter anschalten
    assert_now(&on, "on", true);                                            //ON sollte jetzt true sein
}

//test2
	/* Wir testen, ob wir den Roboter über das Bedienpanel ausschalten können.
	 * -Der Bauer kann den Roboter jederzeit über ein Eingabepanel ein-und ausschalten (btn_start, btn_stop):
	 */
void TestBench::testTurnOff()
{
    send_signal_fifo(&add_fuel, "add_fuel", 100);                           //100ml Treibstoff auffüllen
    send_signal(&plugged_in, "plugged_in", true);                           //Ladekabel ansschließen
    wait(60, SC_SEC);                                                       //für 60s aufladen; Akku = 100%
    send_signal(&plugged_in, "plugged_in", false);                          //Ladekabel trennen
    send_signal_fifo(&btn_start, "btn_start", true);                        //Roboter anschalten
    assert_now(&on, "on", true);                                            //ON sollte jetzt true sein
    send_signal_fifo(&btn_stop, "btn_stop", true);                          //Roboter ausschalten
    assert_now(&on, "on", false);                                           //ON sollte jetzt false sein
}

//test3
	/* Wir testen, ob wir den Roboter ohne Akku anschalten können.
	 * -Wenn der Ladestand weniger als 10% beträgt, kann der Roboter nicht eingeschaltet werden. Drückt der Bauer dann also trotzdem auf den Einschaltknopf, wird dies ignoriert.
	 */
void TestBench::testTurnOnWithoutBattery()
{
    send_signal_fifo(&add_fuel, "add_fuel", 100);                           //100ml Treibstoff auffüllen
    send_signal_fifo(&btn_start, "btn_start", true);                        //Roboter anschalten
    assert_now(&on, "on", false);                                           //ON sollte jetzt false sein
}

//test4
	/* Wir testen, ob der Roboter nach 10s fahren eine Linkskurve fahren kann.
	 * -Ist der Roboter eingeschaltet, befindet er sich in einem von vier Bewegungsmodi: {Stehen, Fahrt nach vorne, Linkskurve, Rechtskurve}.
	 * -Der Roboter erfährt über einen Richtungssensor, wie er sich bewegen soll. Immer wenn der Roboter seine Bewegungsrichtung ändern soll, sendet der Richtungssensor einmalig ein entsprechendes Signal
	 */
void TestBench::testTurnLeft()
{
    send_signal_fifo(&add_fuel, "add_fuel", 100);                           //100ml Treibstoff auffüllen
    send_signal(&plugged_in, "plugged_in", true);                           //Ladekabel ansschließen
    wait(60, SC_SEC);                                                       //für 60s aufladen; Akku = 100%
    send_signal(&plugged_in, "plugged_in", false);                          //Ladekabel trennen
    send_signal_fifo(&btn_start, "btn_start", true);                        //Roboter anschalten
    assert_now(&on, "on", true);                                            //ON sollte jetzt true sein

    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus Fahren senden
    send_signal_fifo(&direction, "direction", TestBench::turn_left);        //Befehl zur Linkskurve senden

    assert_now(&blink_left, "blink_left", true);                            //BLINK_LEFT sollte jetzt true sein
}

//test5
	/* Wir testen, ob der Roboter nach 10s fahren eine Rechtskurve fahren kann.
	 * -Ist der Roboter eingeschaltet, befindet er sich in einem von vier Bewegungsmodi: {Stehen, Fahrt nach vorne, Linkskurve, Rechtskurve}.
	 * -Der Roboter erfährt über einen Richtungssensor, wie er sich bewegen soll. Immer wenn der Roboter seine Bewegungsrichtung ändern soll, sendet der Richtungssensor einmalig ein entsprechendes Signal
	 */
void TestBench::testTurnRight()
{
    send_signal_fifo(&add_fuel, "add_fuel", 100);                           //100ml Treibstoff auffüllen
    send_signal(&plugged_in, "plugged_in", true);                           //Ladekabel ansschließen
    wait(60, SC_SEC);                                                       //für 60s aufladen; Akku = 100%
    send_signal(&plugged_in, "plugged_in", false);                          //Ladekabel trennen
    send_signal_fifo(&btn_start, "btn_start", true);                        //Roboter anschalten
    assert_now(&on, "on", true);                                            //ON sollte jetzt true sein

    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus Fahren senden
    send_signal_fifo(&direction, "direction", TestBench::turn_right);       //Befehl zur Rechtskurve senden

    assert_now(&blink_right, "blink_right", true);                          //BLINK_RIGHT sollte jetzt true sein
}

//test6
	/* Wir testen, ob der Roboter in den Notfallmodus geht und anhält
	 * -Wenn ein Lebewesen den Weg blockiert, sendet der Richtungssensor augenblicklich das Signal blocked. Dann muss der Roboter sofort in den Notfallmodus gehen (danger_lights).
	 */
void TestBench::testHaltOnGoStraight()
{
    send_signal_fifo(&add_fuel, "add_fuel", 100);                           //100ml Treibstoff auffüllen
    send_signal(&plugged_in, "plugged_in", true);                           //Ladekabel ansschließen
    wait(60, SC_SEC);                                                       //für 60s aufladen; Akku = 100%
    send_signal(&plugged_in, "plugged_in", false);                          //Ladekabel trennen
    send_signal_fifo(&btn_start, "btn_start", true);                        //Roboter anschalten
    assert_now(&on, "on", true);                                            //ON sollte jetzt true sein

    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus Fahren senden
    send_signal(&blocked, "blocked", true);                                 //Weg ist blockiert
    assert_now(&danger_lights, "danger_lights", true);                      //DANGER_LIGHTS sollte jetzt true sein
    wait(78, SC_SEC);                                                       //78s warten, 26ml im Stand verbraucht, Treibstoff = 74ml
    assert_now(&low_fuel, "low_fuel", false);                               //LOW_FUEL sollte false sein
}

//test7
 	/* Wir testen, ob das Timing für niedrige Äpfel stimmt.
	 * -Der Roboter fährt seinen Saugarm aus, bis er den vom Sensor wahrgenommenen Apfel erreicht hat. Da die Äpfel unterschiedlich hoch hängen, dauert das Ausfahren unterschiedlich lange:
	 * -Niedrige Äpfel: 1 Sekunde
	 */
void TestBench::testLowAppleTiming()
{
    send_signal_fifo(&add_fuel, "add_fuel", 100);                           //100ml Treibstoff auffüllen
    send_signal(&plugged_in, "plugged_in", true);                           //Ladekabel ansschließen
    wait(60, SC_SEC);                                                       //für 60s aufladen; Akku = 100%
    send_signal(&plugged_in, "plugged_in", false);                          //Ladekabel trennen
    send_signal_fifo(&btn_start, "btn_start", true);                        //Roboter anschalten
    assert_now(&on, "on", true);                                            //ON sollte jetzt true sein

    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus Fahren senden
    send_signal(&apple_sensed, "apple_sensed", TestBench::apple_low);       //niedrigen Apfel wahrnehmen
    wait(1000, SC_MS);                                                      //Arm ausfahren
    assert_now(&vacuum_active, "vacuum_active", true);                      //VACUUM_ACTIVE sollte jetzt true sein

    send_signal_fifo(&apple_harvested, "apple_harvested", true);            //Apfel einsaugen
    send_signal(&apple_sensed, "apple_sensed", TestBench::no_apple);        //keinen Apfel mehr wahrnehmen
    wait(1000, SC_MS);                                                      //Arm einfahren
    assert_now(&vacuum_active, "vacuum_active", false);                     //VACUMM_ACTIVE sollte jetzt false sein
}

//test8
	/* Wir testen, ob das Timing für hohe Äpfel stimmt.
	 * -Der Roboter fährt seinen Saugarm aus, bis er den vom Sensor wahrgenommenen Apfel erreicht hat. Da die Äpfel unterschiedlich hoch hängen, dauert das Ausfahren unterschiedlich lange:
	 * -Hohe Äpfel: 5 Sekunden
	 */
void TestBench::testHighAppleTiming()
{
    send_signal_fifo(&add_fuel, "add_fuel", 100);                           //100ml Treibstoff auffüllen
    send_signal(&plugged_in, "plugged_in", true);                           //Ladekabel ansschließen
    wait(60, SC_SEC);                                                       //für 60s aufladen; Akku = 100%
    send_signal(&plugged_in, "plugged_in", false);                          //Ladekabel trennen
    send_signal_fifo(&btn_start, "btn_start", true);                        //Roboter anschalten
    assert_now(&on, "on", true);                                            //ON sollte jetzt true sein

    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus Fahren senden
    send_signal(&apple_sensed, "apple_sensed", TestBench::apple_high);      //hohen Apfel wahrnehmen
    wait(5000, SC_MS);                                                      //Arm ausfahren
    assert_now(&vacuum_active, "vacuum_active", true);                      //VACUUM_ACTIVE sollte jetzt true sein

    send_signal_fifo(&apple_harvested, "apple_harvested", true);            //Apfel einsaugen
    send_signal(&apple_sensed, "apple_sensed", TestBench::no_apple);        //keinen Apfel mehr wahrnehmen
    wait(5000, SC_MS);                                                      //Arm einfahren
    assert_now(&vacuum_active, "vacuum_active", false);                     //VACUUM_ACTIVE sollte jetzt false sein
}

//test9
	/* Wir testen, ob das Timing für sehr hohe Äpfel stimmt.
	 * -Der Roboter fährt seinen Saugarm aus, bis er den vom Sensor wahrgenommenen Apfel erreicht hat. Da die Äpfel unterschiedlich hoch hängen, dauert das Ausfahren unterschiedlich lange:
	 * -Sehr hohe Äpfel: 8 Sekunden
	 */
void TestBench::testVeryHighAppleTiming()
{
    send_signal_fifo(&add_fuel, "add_fuel", 100);                           //100ml Treibstoff auffüllen
    send_signal(&plugged_in, "plugged_in", true);                           //Ladekabel ansschließen
    wait(60, SC_SEC);                                                       //für 60s aufladen; Akku = 100%
    send_signal(&plugged_in, "plugged_in", false);                          //Ladekabel trennen
    send_signal_fifo(&btn_start, "btn_start", true);                        //Roboter anschalten
    assert_now(&on, "on", true);                                            //ON sollte jetzt true sein

    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus Fahren senden
    send_signal(&apple_sensed, "apple_sensed", TestBench::apple_very_high); //sehr hohen Apfel wahrnehmen
    wait(8000, SC_MS);                                                      //Arm ausfahren
    assert_now(&vacuum_active, "vacuum_active", true);                      //VACUUM_ACTIVE sollte jetzt true sein

    send_signal_fifo(&apple_harvested, "apple_harvested", true);            //Apfel einsaugen
    send_signal(&apple_sensed, "apple_sensed", TestBench::no_apple);        //keinen Apfel mehr wahrnehmen
    wait(8000, SC_MS);                                                      //Arm einfahren
    assert_now(&vacuum_active, "vacuum_active", false);                     //VACUUM_ACTIVE sollte jetzt false sein
}

//test10
	/* Wir testen, ob mit zu wenig Akku der Erntevorgang für niedrige Äpfel versucht wird.
	 * -Beträgt der Ladestand weniger als 50%, ignoriert der Roboter hohe und sehr hohe Äpfel. Beträgt der Ladestand weniger als 25%, ignoriert der Roboter zusätzlich auch niedrige Äpfel.
	 */
void TestBench::testLowAppleNotEnoughBattery()
{
    send_signal_fifo(&add_fuel, "add_fuel", 100);                           //100ml Treibstoff auffüllen
    send_signal(&plugged_in, "plugged_in", true);                           //Ladekabel ansschließen
    wait(15, SC_SEC);                                                       //für 15s aufladen; Akku = 25%
    send_signal(&plugged_in, "plugged_in", false);                          //Ladekabel trennen
    send_signal_fifo(&btn_start, "btn_start", true);                        //Roboter anschalten
    assert_now(&on, "on", true);                                            //ON sollte jetzt true sein

    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus Fahren senden
    send_signal(&apple_sensed, "apple_sensed", TestBench::apple_low);       //niedrigen Apfel wahrnehmen
    wait(1000, SC_MS);                                                      //theoretisch Arm ausfahren
    assert_now(&vacuum_active, "vacuum_active", false);                     //VACUUM_ACTIVE sollte false sein
}

//test11
	/* Wir testen, ob mit zu wenig Akku der Erntevorgang für hohe Äpfel versucht wird.
	 * -Beträgt der Ladestand weniger als 50%, ignoriert der Roboter hohe und sehr hohe Äpfel. Beträgt der Ladestand weniger als 25%, ignoriert der Roboter zusätzlich auch niedrige Äpfel.
	 */
void TestBench::testHighAppleNotEnoughBattery()
{
    send_signal_fifo(&add_fuel, "add_fuel", 100);                           //100ml Treibstoff auffüllen
    send_signal(&plugged_in, "plugged_in", true);                           //Ladekabel ansschließen
    wait(30, SC_SEC);                                                       //für 30s aufladen; Akku = 50%
    send_signal(&plugged_in, "plugged_in", false);                          //Ladekabel trennen
    send_signal_fifo(&btn_start, "btn_start", true);                        //Roboter anschalten
    assert_now(&on, "on", true);                                            //ON sollte jetzt true sein

    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus Fahren senden
    send_signal(&apple_sensed, "apple_sensed", TestBench::apple_high);      //hohen Apfel wahrnehmen
    wait(5000, SC_MS);                                                      //theoretisch Arm ausfahren
    assert_now(&vacuum_active, "vacuum_active", false);                     //VACUUM_ACTIVE sollte jetzt false sein
}

//test12
	/* Wir testen, ob mit zu wenig Akku der Erntevorgang für sehr hohe Äpfel versucht wird.
	 * -Beträgt der Ladestand weniger als 50%, ignoriert der Roboter hohe und sehr hohe Äpfel. Beträgt der Ladestand weniger als 25%, ignoriert der Roboter zusätzlich auch niedrige Äpfel.
	 */
void TestBench::testVeryHighAppleNotEnoughBattery()
{
    send_signal_fifo(&add_fuel, "add_fuel", 100);                           //100ml Treibstoff auffüllen
    send_signal(&plugged_in, "plugged_in", true);                           //Ladekabel ansschließen
    wait(30, SC_SEC);                                                       //für 30s aufladen; Akku = 50%
    send_signal(&plugged_in, "plugged_in", false);                          //Ladekabel trennen
    send_signal_fifo(&btn_start, "btn_start", true);                        //Roboter anschalten
    assert_now(&on, "on", true);                                            //ON sollte jetzt true sein

    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus Fahren senden
    send_signal(&apple_sensed, "apple_sensed", TestBench::apple_very_high); //sehr hohen Apfel wahrnehmen
    wait(8000, SC_MS);                                                      //theoretisch Arm ausfahren
    assert_now(&vacuum_active, "vacuum_active", false);                     //VACUUM_ACTIVE sollte jetzt false sein
}

//test13
	/* Wir testen, ob der Akkuverbrauch für den Erntevorgang für niedrige Äpfel stimmt.
	 * -Die insgesamt verbrauchte Energie zum Aus- oder Einfahren des Saugarms ist jeweils identisch und richtet sich nach der Höhe des zu pflückenden Apfels:
	 * -Niedrige Äpfel: 1% Akku
	 */
void TestBench::testAppleLowBatteryConsumption()
{
    send_signal_fifo(&add_fuel, "add_fuel", 100);                           //100ml Treibstoff auffüllen
    send_signal(&plugged_in, "plugged_in", true);                           //Ladekabel ansschließen
    wait(18, SC_SEC);                                                       //für 18s aufladen; Akku = 30%
    send_signal(&plugged_in, "plugged_in", false);                          //Ladekabel trennen
    send_signal_fifo(&btn_start, "btn_start", true);                        //Roboter anschalten
    assert_now(&on, "on", true);                                            //ON sollte jetzt true sein

    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus Fahren senden
    send_signal(&apple_sensed, "apple_sensed", TestBench::apple_low);       //niedrigen Apfel wahrnehmen
    wait(1000, SC_MS);                                                      //Arm ausfahren; kostet 1%; Akku = 29% 
    assert_now(&low_battery, "low_battery", true);                          //LOW_BATTERY sollte jetzt gesendet werden
}

//test14
	/* Wir testen, ob der Akkuverbrauch für den Erntevorgang für hohe Äpfel stimmt.
	 * -Die insgesamt verbrauchte Energie zum Aus- oder Einfahren des Saugarms ist jeweils identisch und richtet sich nach der Höhe des zu pflückenden Apfels:
	 * -Hohe Äpfel: 6% Akku
	 */
void TestBench::testAppleHighBatteryConsumption()
{
    send_signal_fifo(&add_fuel, "add_fuel", 100);                           //100ml Treibstoff auffüllen
    send_signal(&plugged_in, "plugged_in", true);                           //Ladekabel ansschließen
    wait(33, SC_SEC);                                                       //für 33s aufladen; Akku = 55%
    send_signal(&plugged_in, "plugged_in", false);                          //Ladekabel trennen
    send_signal_fifo(&btn_start, "btn_start", true);                        //Roboter anschalten
    assert_now(&on, "on", true);                                            //ON sollte jetzt true sein

    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus Fahren senden
    send_signal(&apple_sensed, "apple_sensed", TestBench::apple_high);      //hohen Apfel wahrnehmen
    wait(5000, SC_MS);                                                      //Arm ausfahren; kostet 6%; Akku = 49%
    assert_now(&vacuum_active, "vacuum_active", true);                      //VACUUM_ACTIVE sollte jetzt true sein
    wait(6000, SC_MS);                                                      //6s saugen (bis timeout); kostet 20%; Akku = 29%                                   

    assert_now(&low_battery, "low_battery", true);                          //LOW_BATTERY sollte jetzt true sein
}

//test15
	/* Wir testen, ob der Akkuverbrauch für den Erntevorgang für sehr hohe Äpfel stimmt.
	 * -Die insgesamt verbrauchte Energie zum Aus- oder Einfahren des Saugarms ist jeweils identisch und richtet sich nach der Höhe des zu pflückenden Apfels:
	 * -Sehr hohe Äpfel: 11% Akku
	 */
void TestBench::testAppleVeryHighBatteryConsumption()
{
    send_signal_fifo(&add_fuel, "add_fuel", 100);                           //100ml Treibstoff auffüllen
    send_signal(&plugged_in, "plugged_in", true);                           //Ladekabel ansschließen
    wait(33, SC_SEC);                                                       //für 33s aufladen; Akku = 55%
    send_signal(&plugged_in, "plugged_in", false);                          //Ladekabel trennen
    send_signal_fifo(&btn_start, "btn_start", true);                        //Roboter anschalten
    assert_now(&on, "on", true);                                            //ON sollte jetzt true sein

    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus Fahren senden
    send_signal(&apple_sensed, "apple_sensed", TestBench::apple_very_high); //sehr hohen Apfel wahrnehmen
    wait(8000, SC_MS);                                                      //Arm ausfahren; kostet 11%; Akku = 44%
    assert_now(&vacuum_active, "vacuum_active", true);                      //VACUUM_ACTIVE sollte jetzt true sein
    wait(4200, SC_MS);                                                      //4.2s saugen; kostet 15%; Akku = 29%
    send_signal_fifo(&apple_harvested, "apple_harvested", true);            //Apfel einsaugen
    send_signal(&apple_sensed, "apple_sensed", TestBench::no_apple);        //keinen Apfel mehr wahrnehmen

    assert_now(&low_battery, "low_battery", true);                          //LOW_BATTERY sollte jetzt true sein
}

//test16
	/* Wir testen, ob der Akkuverbrauch für den Erntevorgang für niedrige Äpfel stimmt (beim Einfahren).
	 * -Die insgesamt verbrauchte Energie zum Aus- oder Einfahren des Saugarms ist jeweils identisch und richtet sich nach der Höhe des zu pflückenden Apfels:
	 * -Niedrige Äpfel: 1% Akku
	 */
void TestBench::testAppleLowBatteryConsumptionOnWayDown()
{
    send_signal_fifo(&add_fuel, "add_fuel", 100);                           //100ml Treibstoff auffüllen
    send_signal(&plugged_in, "plugged_in", true);                           //Ladekabel ansschließen
    wait(42, SC_SEC);                                                       //für 42s aufladen; Akku = 70%
    send_signal(&plugged_in, "plugged_in", false);                          //Ladekabel trennen
    send_signal_fifo(&btn_start, "btn_start", true);                        //Roboter anschalten
    assert_now(&on, "on", true);                                            //ON sollte jetzt true sein

    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus Fahren senden

    send_signal(&apple_sensed, "apple_sensed", TestBench::apple_low);       //niedrigen Apfel wahrnehmen
    wait(1000, SC_MS);                                                      //Arm ausfahren; kostet 1%; Akku = 69%
    assert_now(&vacuum_active, "vacuum_active", true);                      //VACUUM_ACTIVE sollte jetzt true sein
    wait(1300, SC_MS);                                                      //1.3s saugen; kostet 5%; Akku = 64%
    send_signal_fifo(&apple_harvested, "apple_harvested", true);            //Apfel einsaugen
    send_signal(&apple_sensed, "apple_sensed", TestBench::no_apple);        //keinen Apfel mehr wahrnehmen
    wait(1000, SC_MS);                                                      //Arm einfahren; kostet 1%; Akku = 63%
    assert_now(&vacuum_active, "vacuum_active", false);                     //VACUUM_ACTIVE sollte jetzt false sein
    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus Fahren senden

    send_signal(&apple_sensed, "apple_sensed", TestBench::apple_low);       //niedrigen Apfel wahrnehmen
    wait(1000, SC_MS);                                                      //Arm ausfahren; kostet 1%; Akku = 62%
    assert_now(&vacuum_active, "vacuum_active", true);                      //VACUUM_ACTIVE sollte jetzt true sein
    wait(1300, SC_MS);                                                      //1.3s saugen; kostet 5%; Akku = 57%
    send_signal_fifo(&apple_harvested, "apple_harvested", true);            //Apfel einsaugen
    send_signal(&apple_sensed, "apple_sensed", TestBench::no_apple);        //keinen Apfel mehr wahrnehmen
    wait(1000, SC_MS);                                                      //Arm einfahren; kostet 1%; Akku = 56%
    assert_now(&vacuum_active, "vacuum_active", false);                     //VACUUM_ACTIVE sollte jetzt false sein
    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus Fahren senden

    send_signal(&apple_sensed, "apple_sensed", TestBench::apple_low);       //niedrigen Apfel wahrnehmen
    wait(1000, SC_MS);                                                      //Arm ausfahren; kostet 1%; Akku = 55%
    assert_now(&vacuum_active, "vacuum_active", true);                      //VACUUM_ACTIVE sollte jetzt true sein
    wait(1300, SC_MS);                                                      //1.3s saugen; kostet 5%; Akku = 50%
    send_signal_fifo(&apple_harvested, "apple_harvested", true);            //Apfel einsaugen
    send_signal(&apple_sensed, "apple_sensed", TestBench::no_apple);        //keinen Apfel mehr wahrnehmen
    wait(1000, SC_MS);                                                      //Arm einfahren; kostet 1%; Akku = 49%
    assert_now(&vacuum_active, "vacuum_active", false);                     //VACUUM_ACTIVE sollte jetzt false sein
    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus Fahren senden

    send_signal(&apple_sensed, "apple_sensed", TestBench::apple_low);       //niedrigen Apfel wahrnehmen
    wait(1000, SC_MS);                                                      //Arm ausfahren; kostet 1%; Akku = 48%
    assert_now(&vacuum_active, "vacuum_active", true);                      //VACUUM_ACTIVE sollte jetzt true sein
    wait(1300, SC_MS);                                                      //1.3s saugen; kostet 5%; Akku = 43%
    send_signal_fifo(&apple_harvested, "apple_harvested", true);            //Apfel einsaugen
    send_signal(&apple_sensed, "apple_sensed", TestBench::no_apple);        //keinen Apfel mehr wahrnehmen
    wait(1000, SC_MS);                                                      //Arm einfahren; kostet 1%; Akku = 42%
    assert_now(&vacuum_active, "vacuum_active", false);                     //VACUUM_ACTIVE sollte jetzt false sein
    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus Fahren senden

    send_signal(&apple_sensed, "apple_sensed", TestBench::apple_low);       //niedrigen Apfel wahrnehmen
    wait(1000, SC_MS);                                                      //Arm ausfahren; kostet 1%; Akku = 41%
    assert_now(&vacuum_active, "vacuum_active", true);                      //VACUUM_ACTIVE sollte jetzt true sein
    wait(2200, SC_MS);                                                      //2.2s saugen; kostet 10%; Akku = 31%
    send_signal_fifo(&apple_harvested, "apple_harvested", true);            //Apfel einsaugen
    send_signal(&apple_sensed, "apple_sensed", TestBench::no_apple);        //keinen Apfel mehr wahrnehmen
    wait(1000, SC_MS);                                                      //Arm einfahren; kostet 1%; Akku = 30%
    assert_now(&vacuum_active, "vacuum_active", false);                     //VACUUM_ACTIVE sollte jetzt false sein
    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus Fahren senden

    assert_now(&low_battery, "low_battery", false);                         //LOW_BATTERY sollte jetzt false sein

    send_signal(&apple_sensed, "apple_sensed", TestBench::apple_low);       //niedrigen Apfel wahrnehmen
    wait(1000, SC_MS);                                                      //Arm ausfahren; kostet 1%; Akku = 29%

    assert_now(&low_battery, "low_battery", true);                          //LOW_BATTERY sollte jetzt true sein
}

//test17
	/* Wir testen, ob der Akkuverbrauch für den Erntevorgang für hohe Äpfel stimmt (beim Einfahren).
	 * -Die insgesamt verbrauchte Energie zum Aus- oder Einfahren des Saugarms ist jeweils identisch und richtet sich nach der Höhe des zu pflückenden Apfels:
	 * -Hohe Äpfel: 6% Akku
	 */
void TestBench::testAppleHighBatteryConsumptionOnWayDown()
{
    send_signal_fifo(&add_fuel, "add_fuel", 100);                           //100ml Treibstoff auffüllen
    send_signal(&plugged_in, "plugged_in", true);                           //Ladekabel ansschließen
    wait(48, SC_SEC);                                                       //für 48s aufladen; Akku = 80%
    send_signal(&plugged_in, "plugged_in", false);                          //Ladekabel trennen
    send_signal_fifo(&btn_start, "btn_start", true);                        //Roboter anschalten
    assert_now(&on, "on", true);                                            //ON sollte jetzt true sein

    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus Fahren senden

    send_signal(&apple_sensed, "apple_sensed", TestBench::apple_high);      //hohen Apfel wahrnehmen
    wait(5000, SC_MS);                                                      //Arm ausfahren; kostet 6%; Akku = 74%
    assert_now(&vacuum_active, "vacuum_active", true);                      //VACUUM_ACTIVE sollte jetzt true sein
    wait(1300, SC_MS);                                                      //1.3s saugen; kostet 5%; Akku = 69%
    send_signal_fifo(&apple_harvested, "apple_harvested", true);            //Apfel einsaugen
    send_signal(&apple_sensed, "apple_sensed", TestBench::no_apple);        //keinen Apfel mehr wahrnehmen
    wait(5000, SC_MS);                                                      //Arm einfahren; kostet 6%; Akku = 63%
    assert_now(&vacuum_active, "vacuum_active", false);                     //VACUUM_ACTIVE sollte jetzt false sein
    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus Fahren senden

    send_signal(&apple_sensed, "apple_sensed", TestBench::apple_low);       //niedrigen Apfel wahrnehmen
    wait(1000, SC_MS);                                                      //Arm ausfahren; kostet 1%; Akku = 62%
    assert_now(&vacuum_active, "vacuum_active", true);                      //VACUUM_ACTIVE sollte jetzt true sein
    wait(1300, SC_MS);                                                      //1.3s saugen; kostet 5%; Akku = 57%
    send_signal_fifo(&apple_harvested, "apple_harvested", true);            //Apfel einsaugen
    send_signal(&apple_sensed, "apple_sensed", TestBench::no_apple);        //keinen Apfel mehr wahrnehmen
    wait(1000, SC_MS);                                                      //Arm einfahren; kostet 1%; Akku = 56%
    assert_now(&vacuum_active, "vacuum_active", false);                     //VACUUM_ACTIVE sollte jetzt false sein
    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus Fahren senden

    send_signal(&apple_sensed, "apple_sensed", TestBench::apple_low);       //niedrigen Apfel wahrnehmen
    wait(1000, SC_MS);                                                      //Arm ausfahren; kostet 1%; Akku = 55%
    assert_now(&vacuum_active, "vacuum_active", true);                      //VACUUM_ACTIVE sollte jetzt true sein
    wait(1300, SC_MS);                                                      //1.3s saugen; kostet 5%; Akku = 50%
    send_signal_fifo(&apple_harvested, "apple_harvested", true);            //Apfel einsaugen
    send_signal(&apple_sensed, "apple_sensed", TestBench::no_apple);        //keinen Apfel mehr wahrnehmen
    wait(1000, SC_MS);                                                      //Arm einfahren; kostet 1%; Akku = 49%
    assert_now(&vacuum_active, "vacuum_active", false);                     //VACUUM_ACTIVE sollte jetzt false sein
    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus Fahren senden

    send_signal(&apple_sensed, "apple_sensed", TestBench::apple_low);       //niedrigen Apfel wahrnehmen
    wait(1000, SC_MS);                                                      //Arm ausfahren; kostet 1%; Akku = 48%
    assert_now(&vacuum_active, "vacuum_active", true);                      //VACUUM_ACTIVE sollte jetzt true sein
    wait(1300, SC_MS);                                                      //1.3s saugen; kostet 5%; Akku = 43%
    send_signal_fifo(&apple_harvested, "apple_harvested", true);            //Apfel einsaugen
    send_signal(&apple_sensed, "apple_sensed", TestBench::no_apple);        //keinen Apfel mehr wahrnehmen
    wait(1000, SC_MS);                                                      //Arm einfahren; kostet 1%; Akku = 42%
    assert_now(&vacuum_active, "vacuum_active", false);                     //VACUUM_ACTIVE sollte jetzt false sein
    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus Fahren senden

    send_signal(&apple_sensed, "apple_sensed", TestBench::apple_low);       //niedrigen Apfel wahrnehmen
    wait(1000, SC_MS);                                                      //Arm ausfahren; kostet 1%; Akku = 41%
    assert_now(&vacuum_active, "vacuum_active", true);                      //VACUUM_ACTIVE sollte jetzt true sein
    wait(2200, SC_MS);                                                      //2.2s saugen; kostet 10%; Akku = 31%
    send_signal_fifo(&apple_harvested, "apple_harvested", true);            //Apfel einsaugen
    send_signal(&apple_sensed, "apple_sensed", TestBench::no_apple);        //keinen Apfel mehr wahrnehmen
    wait(1000, SC_MS);                                                      //Arm einfahren; kostet 1%; Akku = 30%
    assert_now(&vacuum_active, "vacuum_active", false);                     //VACUUM_ACTIVE sollte jetzt false sein
    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus Fahren senden

    assert_now(&low_battery, "low_battery", false);                         //LOW_BATTERY sollte jetzt false sein

    send_signal(&apple_sensed, "apple_sensed", TestBench::apple_low);       //niedrigen Apfel wahrnehmen
    wait(1000, SC_MS);                                                      //Arm ausfahren; kostet 1%; Akku = 29%

    assert_now(&low_battery, "low_battery", true);                          //LOW_BATTERY sollte jetzt true sein
}

//test18
	/* Wir testen, ob der Akkuverbrauch für den Erntevorgang für hohe Äpfel stimmt (beim Einfahren).
	 * -Die insgesamt verbrauchte Energie zum Aus- oder Einfahren des Saugarms ist jeweils identisch und richtet sich nach der Höhe des zu pflückenden Apfels:
	 * -Sehr hohe Äpfel: 11% Akku
	 */
void TestBench::testAppleVeryHighBatteryConsumptionOnWayDown()
{
    send_signal_fifo(&add_fuel, "add_fuel", 100);                           //100ml Treibstoff auffüllen
    send_signal(&plugged_in, "plugged_in", true);                           //Ladekabel ansschließen
    wait(54, SC_SEC);                                                       //für 54s aufladen; Akku = 90%
    send_signal(&plugged_in, "plugged_in", false);                          //Ladekabel trennen
    send_signal_fifo(&btn_start, "btn_start", true);                        //Roboter anschalten
    assert_now(&on, "on", true);                                            //ON sollte jetzt true sein
    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus Fahren senden

    send_signal(&apple_sensed, "apple_sensed", TestBench::apple_very_high); //sehr hohen Apfel wahrnehmen
    wait(8000, SC_MS);                                                      //Arm ausfahren; kostet 11%; Akku = 79%
    assert_now(&vacuum_active, "vacuum_active", true);                      //VACUUM_ACTIVE sollte jetzt true sein
    wait(1300, SC_MS);                                                      //1.3s saugen; kostet 5%; Akku = 74%
    send_signal_fifo(&apple_harvested, "apple_harvested", true);            //Apfel einsaugen
    send_signal(&apple_sensed, "apple_sensed", TestBench::no_apple);        //keinen Apfel mehr wahrnehmen
    wait(8000, SC_MS);                                                      //Arm einfahren; kostet 11%; Akku = 63%
    assert_now(&vacuum_active, "vacuum_active", false);                     //VACUUM_ACTIVE sollte jetzt false sein
    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus Fahren senden

    send_signal(&apple_sensed, "apple_sensed", TestBench::apple_low);       //niedrigen Apfel wahrnehmen
    wait(1000, SC_MS);                                                      //Arm ausfahren; kostet 1%; Akku = 62%
    assert_now(&vacuum_active, "vacuum_active", true);                      //VACUUM_ACTIVE sollte jetzt true sein
    wait(1300, SC_MS);                                                      //1.3s saugen; kostet 5%; Akku = 57%
    send_signal_fifo(&apple_harvested, "apple_harvested", true);            //Apfel einsaugen
    send_signal(&apple_sensed, "apple_sensed", TestBench::no_apple);        //keinen Apfel mehr wahrnehmen
    wait(1000, SC_MS);                                                      //Arm einfahren; kostet 1%; Akku = 56%
    assert_now(&vacuum_active, "vacuum_active", false);                     //VACUUM_ACTIVE sollte jetzt false sein
    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus Fahren senden

    send_signal(&apple_sensed, "apple_sensed", TestBench::apple_low);       //niedrigen Apfel wahrnehmen
    wait(1000, SC_MS);                                                      //Arm ausfahren; kostet 1%; Akku = 55%
    assert_now(&vacuum_active, "vacuum_active", true);                      //VACUUM_ACTIVE sollte jetzt true sein
    wait(1300, SC_MS);                                                      //1.3s saugen; kostet 5%; Akku = 50%
    send_signal_fifo(&apple_harvested, "apple_harvested", true);            //Apfel einsaugen
    send_signal(&apple_sensed, "apple_sensed", TestBench::no_apple);        //keinen Apfel mehr wahrnehmen
    wait(1000, SC_MS);                                                      //Arm einfahren; kostet 1%; Akku = 49%
    assert_now(&vacuum_active, "vacuum_active", false);                     //VACUUM_ACTIVE sollte jetzt false sein
    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus Fahren senden

    send_signal(&apple_sensed, "apple_sensed", TestBench::apple_low);       //niedrigen Apfel wahrnehmen
    wait(1000, SC_MS);                                                      //Arm ausfahren; kostet 1%; Akku = 48%
    assert_now(&vacuum_active, "vacuum_active", true);                      //VACUUM_ACTIVE sollte jetzt true sein
    wait(1300, SC_MS);                                                      //1.3s saugen; kostet 5%; Akku = 43%
    send_signal_fifo(&apple_harvested, "apple_harvested", true);            //Apfel einsaugen
    send_signal(&apple_sensed, "apple_sensed", TestBench::no_apple);        //keinen Apfel mehr wahrnehmen
    wait(1000, SC_MS);                                                      //Arm einfahren; kostet 1%; Akku = 42%
    assert_now(&vacuum_active, "vacuum_active", false);                     //VACUUM_ACTIVE sollte jetzt false sein
    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus Fahren senden

    send_signal(&apple_sensed, "apple_sensed", TestBench::apple_low);       //niedrigen Apfel wahrnehmen
    wait(1000, SC_MS);                                                      //Arm ausfahren; kostet 1%; Akku = 41%
    assert_now(&vacuum_active, "vacuum_active", true);                      //VACUUM_ACTIVE sollte jetzt true sein
    wait(2200, SC_MS);                                                      //2.2s saugen; kostet 10%; Akku = 31%
    send_signal_fifo(&apple_harvested, "apple_harvested", true);            //Apfel einsaugen
    send_signal(&apple_sensed, "apple_sensed", TestBench::no_apple);        //keinen Apfel mehr wahrnehmen
    wait(1000, SC_MS);                                                      //Arm einfahren; kostet 1%; Akku = 30%
    assert_now(&vacuum_active, "vacuum_active", false);                     //VACUUM_ACTIVE sollte jetzt false sein
    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus Fahren senden

    assert_now(&low_battery, "low_battery", false);                         //LOW_BATTERY sollte jetzt false sein

    send_signal(&apple_sensed, "apple_sensed", TestBench::apple_low);       //niedrigen Apfel wahrnehmen
    wait(1000, SC_MS);                                                      //Arm ausfahren; kostet 1%; Akku = 29%

    assert_now(&low_battery, "low_battery", true);                          //LOW_BATTERY sollte jetzt true sein
}

//test19
	/* Wir testen, ob der/das Timeout beim Erntevorgang funktioniert
	 * -Konnte der Roboter nach 6 Sekunden Saugzeit den Apfel nicht ernten, bricht er den Vorgang ab und fährt den Arm ebenfalls ein.
	 */
void TestBench::testAppleHarvestingTimout()
{
    send_signal_fifo(&add_fuel, "add_fuel", 100);                           //100ml Treibstoff auffüllen
    send_signal(&plugged_in, "plugged_in", true);                           //Ladekabel ansschließen
    wait(60, SC_SEC);                                                       //für 60s aufladen; Akku = 100%
    send_signal(&plugged_in, "plugged_in", false);                          //Ladekabel trennen
    send_signal_fifo(&btn_start, "btn_start", true);                        //Roboter anschalten
    assert_now(&on, "on", true);                                            //ON sollte jetzt true sein
    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus Fahren senden

    send_signal(&apple_sensed, "apple_sensed", TestBench::apple_low);       //niedrigen Apfel wahrnehmen
    wait(1000, SC_MS);                                                      //Arm ausfahren
    assert_now(&vacuum_active, "vacuum_active", true);                      //VACUUM_ACTIVE sollte jetzt true sein
    wait(7000, SC_MS);                                                      //6s saugen, bis timeout; Arm einfahren
    assert_now(&vacuum_active, "vacuum_active", false);                     //VACUUM_ACTIVE sollte jetzt false sein
}

//test20
	/* Wir testen, ob das Timing für <2s Ernte geht.
	 * -Hat der Saugarm den Apfel erreicht, startet er den Saugvorgang (vacuum_active). Die Dauer des Saugvorgangs variiert, da die Äpfel unterschiedlich fest am Baum sitzen.
	 * -Hat der Saugarm den Apfel erfolgreich geerntet, erhält der Roboter das Signal apple_harvested.
	 * -Die Dauer des Einfahrens entspricht in beiden Fällen der Dauer des Ausfahrens.
	 * -Hat der Roboter den Arm eingefahren, beendet er den Erntemodus. Dazu sendet er das Signal vacuum_idle
	 */
void TestBench::testAppleHarvestingEasy()
{
    send_signal_fifo(&add_fuel, "add_fuel", 100);                           //100ml Treibstoff auffüllen
    send_signal(&plugged_in, "plugged_in", true);                           //Ladekabel ansschließen
    wait(60, SC_SEC);                                                       //für 60s aufladen; Akku = 100%
    send_signal(&plugged_in, "plugged_in", false);                          //Ladekabel trennen
    send_signal_fifo(&btn_start, "btn_start", true);                        //Roboter anschalten
    assert_now(&on, "on", true);                                            //ON sollte jetzt true sein
    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus Fahren senden

    send_signal(&apple_sensed, "apple_sensed", TestBench::apple_low);       //niedrigen Apfel wahrnehmen
    wait(1000, SC_MS);                                                      //Arm ausfahren
    assert_now(&vacuum_active, "vacuum_active", true);                      //VACUUM_ACTIVE sollte jetzt true sein
    wait(1300, SC_MS);                                                      //1.3s saugen
    send_signal_fifo(&apple_harvested, "apple_harvested", true);            //Apfel einsaugen
    send_signal(&apple_sensed, "apple_sensed", TestBench::no_apple);        //keinen Apfel mehr wahrnehmen
    wait(1000, SC_MS);                                                      //Arm einfahren
    assert_now(&vacuum_active, "vacuum_active", false);                     //VACUUM_ACTIVE sollte jetzt false sein
}

//test21
	/* Wir testen, ob das Timing für <4s Ernte geht.
	 * -Hat der Saugarm den Apfel erreicht, startet er den Saugvorgang (vacuum_active). Die Dauer des Saugvorgangs variiert, da die Äpfel unterschiedlich fest am Baum sitzen.
	 * -Hat der Saugarm den Apfel erfolgreich geerntet, erhält der Roboter das Signal apple_harvested.
	 * -Die Dauer des Einfahrens entspricht in beiden Fällen der Dauer des Ausfahrens.
	 * -Hat der Roboter den Arm eingefahren, beendet er den Erntemodus. Dazu sendet er das Signal vacuum_idle
	 */
void TestBench::testAppleHarvestingHard()
{
    send_signal_fifo(&add_fuel, "add_fuel", 100);                           //100ml Treibstoff auffüllen
    send_signal(&plugged_in, "plugged_in", true);                           //Ladekabel ansschließen
    wait(60, SC_SEC);                                                       //für 60s aufladen; Akku = 100%
    send_signal(&plugged_in, "plugged_in", false);                          //Ladekabel trennen
    send_signal_fifo(&btn_start, "btn_start", true);                        //Roboter anschalten
    assert_now(&on, "on", true);                                            //ON sollte jetzt true sein
    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus Fahren senden

    send_signal(&apple_sensed, "apple_sensed", TestBench::apple_low);       //niedrigen Apfel wahrnehmen
    wait(1000, SC_MS);                                                      //Arm ausfahren
    assert_now(&vacuum_active, "vacuum_active", true);                      //VACUUM_ACTIVE sollte jetzt true sein
    wait(2300, SC_MS);                                                      //2.3s saugen
    send_signal_fifo(&apple_harvested, "apple_harvested", true);            //Apfel einsaugen
    send_signal(&apple_sensed, "apple_sensed", TestBench::no_apple);        //keinen Apfel mehr wahrnehmen
    wait(1000, SC_MS);                                                      //Arm einfahren
    assert_now(&vacuum_active, "vacuum_active", false);                     //VACUUM_ACTIVE sollte jetzt false sein
}

//test22
	/* Wir testen, ob das Timing für <6s Ernte geht.
	 * -Hat der Saugarm den Apfel erreicht, startet er den Saugvorgang (vacuum_active). Die Dauer des Saugvorgangs variiert, da die Äpfel unterschiedlich fest am Baum sitzen.
	 * -Hat der Saugarm den Apfel erfolgreich geerntet, erhält der Roboter das Signal apple_harvested.
	 * -Die Dauer des Einfahrens entspricht in beiden Fällen der Dauer des Ausfahrens.
	 * -Hat der Roboter den Arm eingefahren, beendet er den Erntemodus. Dazu sendet er das Signal vacuum_idle
	 */
void TestBench::testAppleHarvestingVeryHard()
{
    send_signal_fifo(&add_fuel, "add_fuel", 100);                           //100ml Treibstoff auffüllen
    send_signal(&plugged_in, "plugged_in", true);                           //Ladekabel ansschließen
    wait(60, SC_SEC);                                                       //für 60s aufladen; Akku = 100%
    send_signal(&plugged_in, "plugged_in", false);                          //Ladekabel trennen
    send_signal_fifo(&btn_start, "btn_start", true);                        //Roboter anschalten
    assert_now(&on, "on", true);                                            //ON sollte jetzt true sein
    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus Fahren senden

    send_signal(&apple_sensed, "apple_sensed", TestBench::apple_low);       //niedrigen Apfel wahrnehmen
    wait(1000, SC_MS);                                                      //Arm ausfahren
    assert_now(&vacuum_active, "vacuum_active", true);                      //VACUUM_ACTIVE sollte jetzt true sein
    wait(4300, SC_MS);                                                      //4.3s saugen
    send_signal_fifo(&apple_harvested, "apple_harvested", true);            //Apfel einsaugen
    send_signal(&apple_sensed, "apple_sensed", TestBench::no_apple);        //keinen Apfel mehr wahrnehmen
    wait(1000, SC_MS);                                                      //Arm einfahren
    assert_now(&vacuum_active, "vacuum_active", false);                     //VACUUM_ACTIVE sollte jetzt false sein
}

//test23
	/* Wir testen, ob der Akkuverbrauch für die <2s ernte stimmt.
	 * -Die verbrauchte Energie während des Saugvorgangs richtet sich nach dessen Dauer:
	 * -Weniger als 2 Sekunden: 5% Akku
	 */
void TestBench::testAppleHarvestingEasyBatteryConsumption()
{
    send_signal_fifo(&add_fuel, "add_fuel", 100);                           //100ml Treibstoff auffüllen
    send_signal(&plugged_in, "plugged_in", true);                           //Ladekabel ansschließen
    wait(21, SC_SEC);                                                       //für 21s aufladen; Akku = 35%
    send_signal(&plugged_in, "plugged_in", false);                          //Ladekabel trennen
    send_signal_fifo(&btn_start, "btn_start", true);                        //Roboter anschalten
    assert_now(&on, "on", true);                                            //ON sollte jetzt true sein
    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus Fahren senden

    send_signal(&apple_sensed, "apple_sensed", TestBench::apple_low);       //niedrigen Apfel wahrnehmen
    wait(1000, SC_MS);                                                      //Arm ausfahren; kostet 1%; Akku = 34%
    assert_now(&vacuum_active, "vacuum_active", true);                      //VACUUM_ACTIVE sollte jetzt true sein
    wait(1000, SC_MS);                                                      //1s saugen; kostet 5%; Akku = 29%
    send_signal_fifo(&apple_harvested, "apple_harvested", true);            //Apfel einsaugen
    send_signal(&apple_sensed, "apple_sensed", TestBench::no_apple);        //keinen Apfel mehr wahrnehmen
    assert_now(&low_battery, "low_battery", true);                          //LOW_BATTERY sollte jetzt true sein
}

//test24
	/* Wir testen, ob der Akkuverbrauch für die <4s ernte stimmt.
	 * -Die verbrauchte Energie während des Saugvorgangs richtet sich nach dessen Dauer:
	 * -Zwischen 2 (inkl.) und 4 (exkl.) Sekunden: 10% Akku
	 */
void TestBench::testAppleHarvestingHardBatteryConsumption()
{
    send_signal_fifo(&add_fuel, "add_fuel", 100);                           //100ml Treibstoff auffüllen
    send_signal(&plugged_in, "plugged_in", true);                           //Ladekabel ansschließen
    wait(24, SC_SEC);                                                       //für 24s aufladen; Akku = 40%
    send_signal(&plugged_in, "plugged_in", false);                          //Ladekabel trennen
    send_signal_fifo(&btn_start, "btn_start", true);                        //Roboter anschalten
    assert_now(&on, "on", true);                                            //ON sollte jetzt true sein
    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus Fahren senden

    send_signal(&apple_sensed, "apple_sensed", TestBench::apple_low);       //niedrigen Apfel wahrnehmen
    wait(1000, SC_MS);                                                      //Arm ausfahren; kostet 1%; Akku = 39%
    assert_now(&vacuum_active, "vacuum_active", true);                      //VACUUM_ACTIVE sollte jetzt true sein
    wait(2300, SC_MS);                                                      //2.3s saugen, kostet 10%; Akku = 29%
    send_signal_fifo(&apple_harvested, "apple_harvested", true);            //Apfel einsaugen
    send_signal(&apple_sensed, "apple_sensed", TestBench::no_apple);        //keinen Apfel mehr wahrnehmen
    assert_now(&low_battery, "low_battery", true);                          //LOW_BATTERY sollte jetzt true sein
}

//test25
	/* Wir testen, ob der Akkuverbrauch für die <6s ernte stimmt.
	 * -Die verbrauchte Energie während des Saugvorgangs richtet sich nach dessen Dauer:
	 * -Zwischen 4 (inkl.) und 6 (exkl.) Sekunden: 15% Akku
	 */
void TestBench::testAppleHarvestingVeryHardBatteryConsumption()
{
    send_signal_fifo(&add_fuel, "add_fuel", 100);                           //100ml Treibstoff auffüllen
    send_signal(&plugged_in, "plugged_in", true);                           //Ladekabel ansschließen
    wait(27, SC_SEC);                                                       //für 27s aufladen; Akku = 45%
    send_signal(&plugged_in, "plugged_in", false);                          //Ladekabel trennen
    send_signal_fifo(&btn_start, "btn_start", true);                        //Roboter anschalten
    assert_now(&on, "on", true);                                            //ON sollte jetzt true sein
    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus Fahren senden

    send_signal(&apple_sensed, "apple_sensed", TestBench::apple_low);       //niedrigen Apfel wahrnehmen
    wait(1000, SC_MS);                                                      //Arm ausfahren; kostet 1%; Akku = 44%
    assert_now(&vacuum_active, "vacuum_active", true);                      //VACUUM_ACTIVE sollte jetzt true sein
    wait(4200, SC_MS);                                                      //4.2s saugen; kostet 15%; Akku = 29%
    send_signal_fifo(&apple_harvested, "apple_harvested", true);            //Apfel einsaugen
    send_signal(&apple_sensed, "apple_sensed", TestBench::no_apple);        //keinen Apfel mehr wahrnehmen
    assert_now(&low_battery, "low_battery", true);                          //LOW_BATTERY sollte jetzt true sein
}

//test26
	/* Wir testen, ob der Akkuverbrauch für den/das timeout stimmt.
	 * -Die verbrauchte Energie während des Saugvorgangs richtet sich nach dessen Dauer:
	 * -6 Sekunden: 20% Akku 
	 */
void TestBench::testAppleHarvestingTimeoutBatteryConsumption()
{
    send_signal_fifo(&add_fuel, "add_fuel", 100);                           //100ml Treibstoff auffüllen
    send_signal(&plugged_in, "plugged_in", true);                           //Ladekabel ansschließen
    wait(30, SC_SEC);                                                       //für 30s aufladen; Akku = 50%
    send_signal(&plugged_in, "plugged_in", false);                          //Ladekabel trennen
    send_signal_fifo(&btn_start, "btn_start", true);                        //Roboter anschalten
    assert_now(&on, "on", true);                                            //ON sollte jetzt true sein
    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus Fahren senden

    send_signal(&apple_sensed, "apple_sensed", TestBench::apple_low);       //niedrigen Apfel wahrnehmen
    wait(1000, SC_MS);                                                      //Arm ausfahren; kostet 1%; Akku = 49%
    assert_now(&vacuum_active, "vacuum_active", true);                      //VACUUM_ACTIVE sollte jetzt true sein
    wait(6000, SC_MS);                                                      //6s saugen bis timeout; kostet 20%; Akku = 39%
    assert_now(&low_battery, "low_battery", true);                          //LOW_BATTERY sollte jetzt true sein
}

//test27
	/* Wir testen, ob der Treibstoffverbrauch im stehen stimmt.
	 * -Während der Fahrt verbraucht der Motor alle 3 Sekunden 2ml Treibstoff. Im Stand verbraucht der Motor alle 3 Sekunden 1ml Treibstoff.
	 */
void TestBench::testFuelConsumptionWhileStanding()
{
    send_signal_fifo(&add_fuel, "add_fuel", 100);                           //100ml Treibstoff auffüllen
    send_signal(&plugged_in, "plugged_in", true);                           //Ladekabel ansschließen
    wait(60, SC_SEC);                                                       //für 60s aufladen; Akku = 100%
    send_signal(&plugged_in, "plugged_in", false);                          //Ladekabel trennen
    send_signal_fifo(&btn_start, "btn_start", true);                        //Roboter anschalten
    assert_now(&on, "on", true);                                            //ON sollte jetzt true sein
    wait(150, SC_SEC);                                                      //150s stehen; kostet 50ml; Tankinhalt = 50ml
    assert_now(&low_fuel, "low_fuel", false);                               //LOW_FUEL sollte jetzt false sein

    wait(3, SC_SEC);                                                        //3s stehen; kostet 1ml; Tankinhalt = 49ml
    assert_now(&low_fuel, "low_fuel", true);                                //LOW_FUEL sollte jetzt true
}

//test28
	/* Wir testen, ob der Roboter mit weniger, als 50ml angeschaltet werden kann
	 * -Damit der Roboter gestartet werden kann muss er jetzt auch mindestens 50ml Treibstoff im Tank haben
	 */
void TestBench::testStartBelow50ml()
{
    send_signal_fifo(&add_fuel, "add_fuel", 49);                            //49ml Treibstoff auffüllen
    send_signal(&plugged_in, "plugged_in", true);                           //Ladekabel ansschließen
    wait(60, SC_SEC);                                                       //für 60s aufladen; Akku = 100%
    send_signal(&plugged_in, "plugged_in", false);                          //Ladekabel trennen
    send_signal_fifo(&btn_start, "btn_start", true);                        //Roboter anschalten
    assert_now(&on, "on", false);                                           //ON sollte jetzt false sein
}

//test29
	/* Wir testen, ob der Treibstoffverbrauch im Fahren stimmt. Außerdem Testen wir, ob der Roboter sich bei einem Tankinhalt von weniger als 10ml ausschaltet.
     * -Wenn der Tankinhalt unter 10ml fällt, schaltet sich der Roboter ab.
	 */
void TestBench::testTankEmpty100ml()
{
    send_signal_fifo(&add_fuel, "add_fuel", 100);                           //100ml Treibstoff auffüllen
    send_signal(&plugged_in, "plugged_in", true);                           //Ladekabel ansschließen
    wait(60, SC_SEC);                                                       //für 60s aufladen; Akku = 100%
    send_signal(&plugged_in, "plugged_in", false);                          //Ladekabel trennen
    send_signal_fifo(&btn_start, "btn_start", true);                        //Roboter anschalten
    assert_now(&on, "on", true);                                            //ON sollte jetzt true sein
    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus fahren senden
    wait(135, SC_SEC);                                                      //135s fahren; kostet 90ml; Tankinhalt = 10ml
    assert_now(&on, "on", true);                                            //ON sollte jetzt true sein
    wait(3, SC_SEC);                                                        //3s fahren; kostet 2ml; Tankinhalt = 8ml
    assert_now(&on, "on", false);                                           //ON sollte jetzt false sein
}

//test30
	/* Wir testen, ob der Treibstoffverbrauch im Fahren stimmt. Außerdem Testen wir, ob das das vollständige Auffüllen mit 500ml funktioniert
	 * -Der Roboter lagert Treibstoff in einem integriertem Tank. Der Tank kann maximal 500ml Treibstoff auf einmal halten. Zu Beginn ist der Tank leer.
	 * -Während der Fahrt verbraucht der Motor alle 3 Sekunden 2ml Treibstoff. Im Stand verbraucht der Motor alle 3 Sekunden 1ml Treibstoff.
	 */
void TestBench::testTankEmpty500ml()
{
    send_signal_fifo(&add_fuel, "add_fuel", 500);                           //500ml Treibstoff auffüllen
    send_signal(&plugged_in, "plugged_in", true);                           //Ladekabel ansschließen
    wait(60, SC_SEC);                                                       //für 60s aufladen; Akku = 100%
    send_signal(&plugged_in, "plugged_in", false);                          //Ladekabel trennen
    send_signal_fifo(&btn_start, "btn_start", true);                        //Roboter anschalten
    assert_now(&on, "on", true);                                            //ON sollte jetzt true sein
    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus fahren senden
    wait(735, SC_SEC);                                                      //735s fahren; kostet 490ml; Tankinhalt = 10ml
    assert_now(&on, "on", true);                                            //ON sollte jetzt true sein
    wait(3, SC_SEC);                                                        //3s fahren; kostet 2ml; Tankinhalt = 8ml
    assert_now(&on, "on", false);                                           //ON sollte jetzt false sein
}

//test31
	/* Wir testen, ob das Warnsignal LOW_FUEL korrekt funktioniert.
	 * -Wenn der Tankinhalt unter 50ml fällt, soll das Signal low_fuel gesendet werden.
	 */
void TestBench::testTankLowFuel()
{
    send_signal_fifo(&add_fuel, "add_fuel", 100);                           //100ml Treibstoff auffüllen
    send_signal(&plugged_in, "plugged_in", true);                           //Ladekabel ansschließen
    wait(60, SC_SEC);                                                       //für 60s aufladen; Akku = 100%
    send_signal(&plugged_in, "plugged_in", false);                          //Ladekabel trennen
    send_signal_fifo(&btn_start, "btn_start", true);                        //Roboter anschalten
    assert_now(&on, "on", true);                                            //ON sollte jetzt true sein
    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus fahren senden
    wait(75, SC_SEC);                                                       //75s fahren; kostet 50ml; Tankinhalt = 50ml
    assert_now(&low_fuel, "low_fuel", false);                               //LOW_FUEL sollte jetzt false sein
    wait(3, SC_SEC);                                                        //3s fahren; kostet 2ml; Tankinhalt = 48ml
    assert_now(&low_fuel, "low_fuel", true);                                //LOW_FUEL sollte jetzt true sein
}

//test32
	/* Wir testen, ob der Roboter sich ausschaltet und DONE sendet, wenn er 100 Äpfel gesammelt hat.
	 * -Hat der Roboter 100 Äpfel geerntet, ist sein Stauraum voll. Dann schaltet er sich aus und sendet das Signal done.
	 */
void TestBench::testCollect100Apples()
{
    send_signal_fifo(&add_fuel, "add_fuel", 500);                           //500ml Treibstoff auffüllen
    send_signal(&plugged_in, "plugged_in", true);                           //Ladekabel ansschließen
    wait(60, SC_SEC);                                                       //für 60s aufladen; Akku = 100%
    send_signal(&plugged_in, "plugged_in", false);                          //Ladekabel trennen
    send_signal_fifo(&btn_start, "btn_start", true);                        //Roboter anschalten
    assert_now(&on, "on", true);                                            //ON sollte jetzt true sein
    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus fahren senden

    for (int i = 0; i < 99; i++)                                            //99 Iterationen; 99 Äpfel sammeln
    {
        send_signal(&apple_sensed, "apple_sensed", TestBench::apple_low);   //niedrigen Apfel wahrnehmen
        wait(1000, SC_MS);                                                  //Arm ausfahren
        assert_now(&vacuum_active, "vacuum_active", true);                  //VACUUM_ACTIVE sollte jetzt true sein
        wait(1300, SC_MS);                                                  //1.3s saugen
        send_signal_fifo(&apple_harvested, "apple_harvested", true);        //Apfel einsaugen
        send_signal(&apple_sensed, "apple_sensed", TestBench::no_apple);    //keinen Apfel mehr wahrnehmen
        wait(1000, SC_MS);                                                  //Arm einfahren
        assert_now(&vacuum_active, "vacuum_active", false);                 //VACUUM_ACTIVE sollte jetzt false sein
        send_signal_fifo(&direction, "direction", TestBench::go_straight);  //Befehl zum geradeaus Fahren senden
        send_signal_fifo(&add_fuel, "add_fuel", 500);                       //Treibstoff nachfüllen
        wait(120, SC_SEC);                                                  //Akku aufladen
    }

    send_signal(&apple_sensed, "apple_sensed", TestBench::apple_low);       //niedrigen Apfel wahrnehmen
    wait(1000, SC_MS);                                                      //Arm ausfahren
    assert_now(&vacuum_active, "vacuum_active", true);                      //VACUUM_ACTIVE sollte jetzt true sein
    wait(1300, SC_MS);                                                      //1.3s saugen
    send_signal_fifo(&apple_harvested, "apple_harvested", true);            //Apfel einsaugen
    send_signal(&apple_sensed, "apple_sensed", TestBench::no_apple);        //keinen Apfel mehr wahrnehmen
    assert_now(&done, "done", true);                                        //DONE sollte jetzt true sein
    assert_now(&on, "on", false);                                           //ON sollte jetzt false sein
}

//test33
    /* Wir testen den Initialzustand.
	 */
void TestBench::testInitialState()
{
    assert_now(&blink_left, "blink_left", false);
    assert_now(&blink_right, "blink_right", false);
    assert_now(&danger_lights, "danger_lights", false);
    assert_now(&on, "on", false);
    assert_now(&vacuum_active, "vacuum_active", false);
    assert_now(&low_battery, "low_battery", true);
    assert_now(&low_fuel, "low_fuel", true);
    assert_now(&done, "done", false);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//test01 successful
    
      //Während der Roboter erntet, steht er und sollte nicht auf Signale des Richtungssensors reagieren.
     
void TestBench::testDirectionIgnoredWhileHarvesting()
{
    send_signal_fifo(&add_fuel, "add_fuel", 500);                           //500ml Treibstoff auffüllen
    send_signal(&plugged_in, "plugged_in", true);                           //Ladekabel ansschließen
    wait(60, SC_SEC);                                                       //für 60s aufladen; Akku = 100%
    send_signal(&plugged_in, "plugged_in", false);                          //Ladekabel trennen
    send_signal_fifo(&btn_start, "btn_start", true);                        //Roboter anschalten
    assert_now(&on, "on", true);                                            //ON sollte jetzt true sein
    send_signal_fifo(&direction, "direction", TestBench::go_straight);      //Befehl zum geradeaus fahren senden
    send_signal(&apple_sensed, "apple_sensed", TestBench::apple_low);       //niedrigen Apfel wahrnehmen
    send_signal_fifo(&direction, "direction", TestBench::turn_left);        // Signal zum Abbiegen nach links senden - sollte ignoriert werden
    assert_now(&blink_left, "blink_left", false);                           // BLINK_LEFT sollte hier nicht gesendet werden (weil der Roboter gerade erntet)                                     //ON sollte jetzt false sein
}

//test02 successful
    //TANK: wenn der Tank leer ist, schaltet der Roboter sich automatisch aus.

void TestBench::testSwitchOnWithEmptyTank()
{
    send_signal(&plugged_in, "plugged_in", true);                           //Ladekabel ansschließen
    wait(60, SC_SEC);                                                       //für 60s aufladen; Akku = 100%
    send_signal(&plugged_in, "plugged_in", false);                          //Ladekabel trennen
    send_signal_fifo(&btn_start, "btn_start", true);						//Roboter anschalten
    wait(1, SC_SEC);
    assert_now(&on, "on", false);                        
    send_signal_fifo(&direction, "direction", TestBench::turn_left);        // Signal zum Abbiegen nach links senden - sollte ignoriert werden
    assert_now(&blink_left, "blink_left", false);                           // BLINK_LEFT sollte hier nicht gesendet werden (weil der Roboter hat kein Treibstoff)                                     //ON sollte jetzt false sein
}

//test03 successful
    //Sinkt der Tank während der Fahrt auf 0, schaltet sich der Roboter automatisch aus.

void TestBench::testAutomaticSwitchOffWhenTankEmpty()
{
    send_signal_fifo(&add_fuel, "add_fuel", 100);
    send_signal(&plugged_in, "plugged_in", true);
    wait(60, SC_SEC);
    send_signal(&plugged_in, "plugged_in", false);
    send_signal_fifo(&btn_start, "btn_start", true);
    assert_now(&on, "on", true); 
    send_signal_fifo(&direction, "direction", TestBench::go_straight);
    wait(150, SC_SEC);
    send_signal_fifo(&direction, "direction", TestBench::turn_left);
    assert_now(&blink_left, "blink_left", false);
}                          

//test04 successful
    // Off Button während Erntevorgang Drücken --> Erntevorgang abschließen, dann ausschalten

void TestBench::testOffButtonWhileHarvesting(){
    send_signal_fifo(&add_fuel, "add_fuel", 500);
    send_signal(&plugged_in, "plugged_in", true);
    wait(60, SC_SEC);
    send_signal(&plugged_in, "plugged_in", false);
    send_signal_fifo(&btn_start, "btn_start", true);
    assert_now(&on, "on", true); 
    send_signal_fifo(&direction, "direction", TestBench::go_straight);
    wait(2, SC_SEC);
    send_signal(&apple_sensed, "apple_sensed", TestBench::apple_low);
    wait(1, SC_SEC);
    assert_now(&vacuum_active, "vacuum_active", true);
    send_signal_fifo(&btn_stop, "btn_stop", true);
    assert_now(&on, "on", true);
    wait(1300, SC_MS);
    send_signal_fifo(&apple_harvested, "apple_harvested", true);
    wait(1, SC_SEC);
    assert_now(&on, "on", false);
}

//test05  testChargingWhileHarvesting fehlt wegen der neuen Annahmen aus.

    /*
        Der Roboter kann Laden wenn er steht und an oder aus ist (also auch im Erntemodus!). 
        Beim Wechsel von "Ein" nach "Aus" und zurück sollte weiter geladen werden, wenn das Kabel angeschlossen ist.
        Einschalten darf nicht möglich sein, wenn der letzte Bewegungszustand vor Ausschalten Geradeaus, Linkskurve 
        oder Rechtskurve war und das Kabel steckt (ob der Roboter in einem dieser Zustände ist bzw. war, bevor er
        ausgeschaltet wurde, wird in der Variable 'R_state' gespeichert).
    */


//test06 successful
    //Wechsel von "Aus/Charging" zu "Ein[Stehen]/Charging

void TestBench::testChargingTransitionOffOn(){
    send_signal_fifo(&add_fuel, "add_fuel", 500);
    send_signal(&plugged_in, "plugged_in", true);
    wait(18, SC_SEC);
    send_signal_fifo(&btn_start, "btn_start", true);
    assert_now(&on, "on", true); 
    wait(12, SC_SEC);
    send_signal(&plugged_in, "plugged_in", false);
    send_signal_fifo(&direction, "direction", TestBench::go_straight);
    wait(1, SC_SEC);
    send_signal(&apple_sensed, "apple_sensed", TestBench::apple_low);
    wait(1, SC_SEC);
    assert_now(&vacuum_active, "vacuum_active", true);
    wait(6, SC_SEC);
    wait(1, SC_NS);
    assert_now(&low_battery, "low_battery", true);
    send_signal_fifo(&btn_stop, "btn_stop", true);
}

//test07 successful
    //Wechsel von "Ein[Stehen]/Charging" zu "Aus/Charging"

void TestBench::testChargingTransitionOnOff(){
    send_signal_fifo(&add_fuel, "add_fuel", 500);
    send_signal(&plugged_in, "plugged_in", true);
    wait(18, SC_SEC);
    send_signal_fifo(&btn_start, "btn_start", true);
    assert_now(&on, "on", true); 
    wait(6, SC_SEC);
    send_signal_fifo(&btn_stop, "btn_stop", true);
    wait(6, SC_SEC);
    send_signal(&plugged_in, "plugged_in", false);
    send_signal_fifo(&btn_start, "btn_start", true);
    assert_now(&on, "on", true);
    send_signal_fifo(&direction, "direction", TestBench::go_straight);
    wait(1, SC_SEC);
    send_signal(&apple_sensed, "apple_sensed", TestBench::apple_low);
    wait(1, SC_SEC);
    assert_now(&vacuum_active, "vacuum_active", true);
    wait(6, SC_SEC);
    wait(1, SC_NS);
    assert_now(&low_battery, "low_battery", true);
    send_signal_fifo(&btn_stop, "btn_stop", true);
}

//test08 successful
    //Einschalten nicht möglich, wenn Roboter zuletzt in einem der Bewegungs-Zustände Geradeaus/Rechtskurve/Linkskurve
    //war und das Kabel eingesteckt ist
void TestBench::testNoReturnToMotionHistoryWithCable(){
    send_signal_fifo(&add_fuel, "add_fuel", 500);
    send_signal(&plugged_in, "plugged_in", true);
    wait(60, SC_SEC);
    send_signal(&plugged_in, "plugged_in", false);
    send_signal_fifo(&btn_start, "btn_start", true);
    assert_now(&on, "on", true); 
    send_signal_fifo(&direction, "direction", TestBench::go_straight);
    wait(5, SC_SEC);
    send_signal_fifo(&btn_stop, "btn_stop", true);
    send_signal(&plugged_in, "plugged_in", true);
    send_signal_fifo(&btn_start, "btn_start", true);
    assert_now(&on, "on", false);
}

//test09 successful
    //Charging in Bewegungsmodus nicht möglich

void TestBench::testNoReturnToMotionFromHarvestingWithCable(){
    send_signal_fifo(&add_fuel, "add_fuel", 500);
    send_signal(&plugged_in, "plugged_in", true);
    wait(30, SC_SEC);
    send_signal(&plugged_in, "plugged_in", false);
    send_signal_fifo(&btn_start, "btn_start", true);
    assert_now(&on, "on", true);
    send_signal(&plugged_in, "plugged_in", true); 
    send_signal_fifo(&direction, "direction", TestBench::turn_left);
    assert_now(&blink_left, "blink_left", false);
}

//test10 successful
void TestBench::testHistoryFunctionality(){
    send_signal_fifo(&add_fuel, "add_fuel", 100);
    send_signal(&plugged_in, "plugged_in", true);
    wait(21, SC_SEC);
    send_signal(&plugged_in, "plugged_in", false);
    send_signal_fifo(&btn_start, "btn_start", true);
    assert_now(&on, "on", true);
    send_signal_fifo(&direction, "direction", TestBench::turn_left);
    wait(5, SC_SEC);
    send_signal_fifo(&btn_stop, "btn_stop", true);
    wait(1, SC_SEC);
    send_signal_fifo(&btn_start, "btn_start", true);
    assert_now(&blink_left, "blink_left", true);
}

//test11: robot entering emergency mode while turning left
void TestBench::testHaltOnTurnLeft(){

    // add 100 ml fuel
    send_signal_fifo(&add_fuel, "add_fuel", 100);      

    // charge for 21 secs: 35 % battery
    send_signal(&plugged_in, "plugged_in", true);
    wait(21, SC_SEC);                                  
    send_signal(&plugged_in, "plugged_in", false);

    // switch on
    send_signal_fifo(&btn_start, "btn_start", true);    
    assert_now(&on, "on", true);

    // turn left & send blocked signal; assert danger_lights is true
    send_signal_fifo(&direction, "direction", TestBench::turn_left);
    send_signal(&blocked, "blocked", true);                                 //Weg ist blockiert
    assert_now(&danger_lights, "danger_lights", true);                      //DANGER_LIGHTS sollte jetzt true sein
}

//test12: robot entering emergency mode while turning right
void TestBench::testHaltOnTurnRight(){

    // add 100 ml fuel
    send_signal_fifo(&add_fuel, "add_fuel", 100);      

    // charge for 21 secs: 35 % battery
    send_signal(&plugged_in, "plugged_in", true);
    wait(21, SC_SEC);                                  
    send_signal(&plugged_in, "plugged_in", false);

    // switch on
    send_signal_fifo(&btn_start, "btn_start", true);    
    assert_now(&on, "on", true);

    // turn left & send blocked signal; assert danger_lights is true
    send_signal_fifo(&direction, "direction", TestBench::turn_right);
    send_signal(&blocked, "blocked", true);                                 //Weg ist blockiert
    assert_now(&danger_lights, "danger_lights", true);                      //DANGER_LIGHTS sollte jetzt true sein
}