COMP 3004 Final Project: Implementing a Tandem Pump Simulation

Team 37
- Devanjali Das: State Machine Diagrams, Sequence Diagrams.
- Gurleen Bassali: Full Frontend Code, Requirements Traceability Matrix.
- Karan Modi: Full Backend Code, Video.
- Zainab Jahangir: Use Cases, Use Case Diagram, Class Diagram, Sequence Diagrams.

Project Description:
   This project is a simulation of the Tandem t:slim X2 Insulin Pump. The simulation demonstates the key features of the pump including:
      - Insulin Delivery: Both basal and bolus insulin injections
      - ControlIQ: Adjusts insulin delivery based on continuous glucose monitoring (CGM) data
      - Profile Management: CRUD system for managing personal insulin delivery profiles (basal rates, carb ratio, correction factor, target glucose)
      - Monitoring and Logging: Real-time feedback on battery status, insulin levels, Insulin on Board (IOB) and glucose levels; logging of events and error handling
      - Timing: CGM measurements occur every 5 real seconds at 30 minute simulation intervals. 1 real second is equivalent to 6 simulated minutes.

File Organization:

- Diagrams: Folder that contains subfolders for diagrams such as State Machine, Sequence and Use Cases
      - Insulin Pump Class Diagram.png - Class Diagram for our project
      - Use Case Diagram.png - Use Case Diagram for our project
      - State Machine Diagrams Folder: (contains 3 state machine diagrams for our project)
            - State Machine #1.png
            - State Machine #2.png
            - State Machine #3.png
      - Sequence Diagrams Folder:
            - Manual_Bolus_Administration_Default_profile.png
            - Bolus_Fetch_From_CGM.png
            - Sequence Diagram #3.png
            - Automatic_Bolus_administration_Sequence_Diagram.png
            - Battery_Warning_Sequence_Diagram.png
            - Insufficient_Insulin_sequence_diagram.png

- QT Code Files: Folder that contains all of the QT Code requierd to run our simulation
       1. battery.cpp & battery.h 
       2. BolusCalculator.cpp & BolusCalculator.h
       3. CGM.cpp & CGM.H
       4. ControlIQ.cpp & ControlIQ.h
       5. logger.cpp & logger.h
       6. main.cpp
       7. mainwindow.cpp & mainwindow.h
       8. mainwindow.ui
       9. Makefile
       10. team_project.pro.user
       11. team_project.pro
       12. test.cpp
- Requirements Traceability Matrix: PDF of our Matrix
- Use Cases for Tandem tX2 Insulin Pump Simulation

Instructions on how to run:
   1. Open terminal and run these commands to install QTCharts:
         - sudo apt update
         - sudo apt install libqt5charts5-dev
   2. Open the project in Qt (using .pro file).
   3. When prompted to enter PIN, enter 1234 (PIN is hardcoded).
  
