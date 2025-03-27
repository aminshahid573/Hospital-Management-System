# Console Hospital Management System in C

This is a simple, console-based Hospital Management System written in C. It allows users to manage patient records, doctor information, appointments, and basic billing through a menu-driven interface. Data is persisted using binary files (`.dat`).

## Features

*   **Patient Management:** Add, View, Edit, Delete patient records.
*   **Doctor Management:** Add, View, Search doctor details (by name/specialization).
*   **Appointment Scheduling:** Book, View, and Cancel appointments linking patients and doctors.
*   **Billing System:** Generate bills (with optional doctor fees), view bills, and print simple invoices.
*   **Data Persistence:** Save and load all data (patients, doctors, appointments, bills) to/from binary `.dat` files.
*   **Menu-Driven Interface:** Easy-to-use console menu for navigation.

## How to Compile and Run

1.  **Compile:** You need a C compiler (like GCC). Save the code as `hospital_management.c` (or any other name).
    ```bash
    gcc hospital_management.c -o hospital_management
    ```
2.  **Run:** Execute the compiled program.
    *   On Linux/macOS:
        ```bash
        ./hospital_management
        ```
    *   On Windows:
        ```bash
        hospital_management.exe
        ```

## Usage & Example Outputs

The program presents a main menu from which you can navigate to different management sections.

**Main Menu:**

```
===== Hospital Management System =====
1. Patient Management
2. Doctor Management
3. Appointment Management
4. Billing System
5. Save Data to Files
0. Exit
======================================
Enter your choice:
```

**Example: Adding a Patient**

1.  Select `1` from the Main Menu.
2.  Select `1` from the Patient Management Menu.

```
--- Patient Management ---
1. Add New Patient
2. View All Patients
3. Edit Patient Information
4. Delete Patient Record
0. Back to Main Menu
Enter your choice: 1

--- Add New Patient ---
Enter Name: Shahid Amin
Enter Age: 35
Enter Gender: Male
Enter Disease/Condition: Flu
Enter Contact Number: 123-456-7890
Patient added successfully with ID: 1
```

**Example: Viewing Patients**

1.  Select `1` from the Main Menu.
2.  Select `2` from the Patient Management Menu.

```
--- Patient Management ---
1. Add New Patient
2. View All Patients
...
Enter your choice: 2

--- Patient List (1) ---
-----------------------------------------------------------------------------------
ID   | Name                 | Age | Gender   | Disease              | Contact
-----------------------------------------------------------------------------------
1    | Shahid Amin          | 35  | Male     | Flu                  | 123-456-7890
-----------------------------------------------------------------------------------
```

**Example: Scheduling an Appointment**

(Assuming Patient ID 1 and Doctor ID 1 exist)

1.  Select `3` from the Main Menu.
2.  Select `1` from the Appointment Management Menu.

```
--- Appointment Management ---
1. Schedule New Appointment
...
Enter your choice: 1

--- Schedule New Appointment ---
--- Patient List (1) ---
... (patient list shown) ...
Enter Patient ID: 1
--- Doctor List (1) ---
... (doctor list shown) ...
Enter Doctor ID: 1
Enter Appointment Date (YYYY-MM-DD): 2023-10-27
Enter Appointment Time (HH:MM): 10:30
Appointment scheduled successfully for Patient Shahid Amin with Dr. Alice Smith on 2023-10-27 at 10:30 (Appt ID: 1)
```

**Example: Generating a Bill and Printing Invoice**

1.  Select `4` from the Main Menu.
2.  Select `1` to Generate Bill.
3.  Select `3` to Print Invoice.

```
--- Billing Management ---
1. Generate New Bill
...
Enter your choice: 1

--- Generate New Bill ---
--- Patient List (1) ---
... (patient list shown) ...
Enter Patient ID for the bill: 1
Is this bill related to a specific doctor consultation? (yes/no): yes
--- Doctor List (1) ---
... (doctor list shown) ...
Enter Doctor ID for the consultation fee: 1
Enter Doctor Consultation Fee: 150.00
Enter Bill Date (YYYY-MM-DD): 2023-10-27
Bill generated successfully for Patient Shahid Amin (Bill ID: 1)
Total Amount: 150.00

--- Billing Management ---
...
3. Print Invoice
...
Enter your choice: 3
Enter Bill ID to print invoice: 1


--- HOSPITAL INVOICE ---
----------------------------------------
 Bill ID       : 1
 Bill Date     : 2023-10-27
----------------------------------------
 Patient Details:
   Patient ID  : 1
   Name        : Shahid Amin
   Age         : 35
   Gender      : Male
   Contact     : 123-456-7890
----------------------------------------
 Charges:
   Doctor Fee (Dr. Alice Smith): 150.00
----------------------------------------
 Total Amount  : 150.00
----------------------------------------
 Thank you!
----------------------------------------

```

**Saving Data:**

Select `5` from the main menu to save all current data to the `.dat` files. Data is also automatically saved when exiting if you choose 'yes'.

```
Enter your choice: 5
Data saved successfully.
```

## File Structure

The application uses the following binary files to store data:

*   `patients.dat`: Stores patient records.
*   `doctors.dat`: Stores doctor records.
*   `appointments.dat`: Stores appointment records.
*   `bills.dat`: Stores bill records.
*   `counters.dat`: Stores the next available ID for each record type to ensure uniqueness.

**Note:** These `.dat` files are binary and not human-readable in a standard text editor.

## Dependencies

*   Standard C Libraries (`stdio.h`, `stdlib.h`, `string.h`)
*   No external libraries are required.


