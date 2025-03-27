#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <ctype.h> // Removed as requested

// --- Constants ---
#define MAX_PATIENTS 100
#define MAX_DOCTORS 50
#define MAX_APPOINTMENTS 200
#define MAX_BILLS 200
#define NAME_LEN 100
#define GENDER_LEN 10
#define DISEASE_LEN 100
#define CONTACT_LEN 15
#define SPECIALIZATION_LEN 100
#define AVAILABILITY_LEN 50
#define DATE_LEN 11 // YYYY-MM-DD
#define TIME_LEN 6  // HH:MM

// --- File Names ---
#define PATIENT_FILE "patients.dat"
#define DOCTOR_FILE "doctors.dat"
#define APPOINTMENT_FILE "appointments.dat"
#define BILL_FILE "bills.dat"
#define COUNTER_FILE "counters.dat" // To store next IDs

// --- Data Structures (Using struct Name {...}; style) ---
struct Patient {
    int id;
    char name[NAME_LEN];
    int age;
    char gender[GENDER_LEN];
    char disease[DISEASE_LEN];
    char contact[CONTACT_LEN];
};

struct Doctor {
    int id;
    char name[NAME_LEN];
    char specialization[SPECIALIZATION_LEN];
    char availability[AVAILABILITY_LEN];
};

struct Appointment {
    int id;
    int patientId;
    int doctorId;
    char date[DATE_LEN]; // YYYY-MM-DD
    char time[TIME_LEN]; // HH:MM
};

struct Bill {
    int id;
    int patientId;
    int doctorId; // Optional: To link doctor fee
    float doctorFee;
    // float medicineCost; // Add if pharmacy is implemented
    // float testCost;     // Add if tests are implemented
    float totalAmount;
    char dateGenerated[DATE_LEN];
};

// --- Application State Structure ---
// Holds all data previously stored in global variables
struct AppState {
    struct Patient patients[MAX_PATIENTS];
    int patientCount;

    struct Doctor doctors[MAX_DOCTORS];
    int doctorCount;

    struct Appointment appointments[MAX_APPOINTMENTS];
    int appointmentCount;

    struct Bill bills[MAX_BILLS];
    int billCount;

    // ID Counters
    int nextPatientId;
    int nextDoctorId;
    int nextAppointmentId;
    int nextBillId;
};

// --- Utility Functions ---

// Clear input buffer
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Get string input safely (modified newline removal)
void getStringInput(char* prompt, char* buffer, int len) {
    printf("%s", prompt);
    if (fgets(buffer, len, stdin) != NULL) {
        // Remove trailing newline if present (alternative to strcspn)
        int readLen = strlen(buffer);
        if (readLen > 0 && buffer[readLen - 1] == '\n') {
            buffer[readLen - 1] = '\0';
        }
    } else {
        // Handle error or EOF
        buffer[0] = '\0';
        clearInputBuffer(); // Clear buffer if fgets failed early
    }
}

// Get integer input safely
int getIntInput(char* prompt) {
    int value;
    char buffer[30];
    int success = 0;
    while (!success) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            // Try to convert to integer
            if (sscanf(buffer, "%d", &value) == 1) {
                // Optional: Add range checks if needed
                success = 1;
            } else {
                printf("Invalid input. Please enter a number.\n");
            }
        } else {
             printf("Error reading input.\n");
             // Decide how to handle EOF or read errors, maybe exit?
             return -1; // Indicate error
        }
    }
    return value;
}


// Get float input safely
float getFloatInput(char* prompt) {
    float value;
    char buffer[30];
    int success = 0;
    while(!success) {
        printf("%s", prompt);
         if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            if (sscanf(buffer, "%f", &value) == 1) {
                if (value >= 0) { // Basic validation
                   success = 1;
                } else {
                    printf("Amount cannot be negative.\n");
                }
            } else {
                 printf("Invalid input. Please enter a valid amount.\n");
            }
         } else {
             printf("Error reading input.\n");
             return -1.0f; // Indicate error
         }
    }
    return value;
}


// --- File Handling Functions (Operate on AppState) ---

void saveCounters(struct AppState* state) {
    FILE *fp = fopen(COUNTER_FILE, "wb");
    if (fp == NULL) {
        perror("Error opening counter file for writing");
        return;
    }
    fwrite(&state->nextPatientId, sizeof(int), 1, fp);
    fwrite(&state->nextDoctorId, sizeof(int), 1, fp);
    fwrite(&state->nextAppointmentId, sizeof(int), 1, fp);
    fwrite(&state->nextBillId, sizeof(int), 1, fp);
    fclose(fp);
}

void loadCounters(struct AppState* state) {
    FILE *fp = fopen(COUNTER_FILE, "rb");
    if (fp == NULL) {
        // If file doesn't exist, start IDs from 1 (first run)
        state->nextPatientId = 1;
        state->nextDoctorId = 1;
        state->nextAppointmentId = 1;
        state->nextBillId = 1;
        //perror("Counter file not found, starting from 1"); // Optional message
        return;
    }
    // Ensure reads are successful before assigning
    fread(&state->nextPatientId, sizeof(int), 1, fp);
    fread(&state->nextDoctorId, sizeof(int), 1, fp);
    fread(&state->nextAppointmentId, sizeof(int), 1, fp);
    fread(&state->nextBillId, sizeof(int), 1, fp);
    fclose(fp);
}


void saveData(struct AppState* state) {
    FILE *fp;

    // Save Patients
    fp = fopen(PATIENT_FILE, "wb");
    if (fp == NULL) { perror("Error opening patient file for writing"); return; }
    fwrite(&state->patientCount, sizeof(int), 1, fp);
    fwrite(state->patients, sizeof(struct Patient), state->patientCount, fp);
    fclose(fp);

    // Save Doctors
    fp = fopen(DOCTOR_FILE, "wb");
    if (fp == NULL) { perror("Error opening doctor file for writing"); return; }
    fwrite(&state->doctorCount, sizeof(int), 1, fp);
    fwrite(state->doctors, sizeof(struct Doctor), state->doctorCount, fp);
    fclose(fp);

    // Save Appointments
    fp = fopen(APPOINTMENT_FILE, "wb");
    if (fp == NULL) { perror("Error opening appointment file for writing"); return; }
    fwrite(&state->appointmentCount, sizeof(int), 1, fp);
    fwrite(state->appointments, sizeof(struct Appointment), state->appointmentCount, fp);
    fclose(fp);

    // Save Bills
    fp = fopen(BILL_FILE, "wb");
    if (fp == NULL) { perror("Error opening bill file for writing"); return; }
    fwrite(&state->billCount, sizeof(int), 1, fp);
    fwrite(state->bills, sizeof(struct Bill), state->billCount, fp);
    fclose(fp);

    // Save Counters
    saveCounters(state);

    printf("Data saved successfully.\n");
}

void loadData(struct AppState* state) {
    FILE *fp;
    int readCount;

    // Initialize counts to 0 before loading
    state->patientCount = 0;
    state->doctorCount = 0;
    state->appointmentCount = 0;
    state->billCount = 0;

    // Load Counters first
    loadCounters(state);

    // Load Patients
    fp = fopen(PATIENT_FILE, "rb");
    if (fp != NULL) {
        if (fread(&readCount, sizeof(int), 1, fp) == 1) { // Check if read was successful
            if (readCount >= 0 && readCount <= MAX_PATIENTS) {
                int actualRead = fread(state->patients, sizeof(struct Patient), readCount, fp);
                state->patientCount = actualRead;
                if (actualRead != readCount) {
                    printf("Warning: Mismatch in expected (%d) and read (%d) patient records.\n", readCount, actualRead);
                }
            } else if (readCount > MAX_PATIENTS) {
                printf("Warning: Patient file contains more records (%d) than MAX_PATIENTS (%d). Loading truncated list.\n", readCount, MAX_PATIENTS);
                state->patientCount = fread(state->patients, sizeof(struct Patient), MAX_PATIENTS, fp);
            } else {
                 printf("Warning: Invalid count (%d) in patient file.\n", readCount);
            }
        } else {
            printf("Warning: Could not read count from patient file.\n");
        }
        fclose(fp);
    } // else: File not found is okay, count remains 0


    // Load Doctors (similar logic)
    fp = fopen(DOCTOR_FILE, "rb");
    if (fp != NULL) {
        if (fread(&readCount, sizeof(int), 1, fp) == 1) {
            if (readCount >= 0 && readCount <= MAX_DOCTORS) {
                int actualRead = fread(state->doctors, sizeof(struct Doctor), readCount, fp);
                state->doctorCount = actualRead;
                 if (actualRead != readCount) {
                     printf("Warning: Mismatch in expected (%d) and read (%d) doctor records.\n", readCount, actualRead);
                 }
            } else if (readCount > MAX_DOCTORS) {
                printf("Warning: Doctor file contains more records (%d) than MAX_DOCTORS (%d). Loading truncated list.\n", readCount, MAX_DOCTORS);
                state->doctorCount = fread(state->doctors, sizeof(struct Doctor), MAX_DOCTORS, fp);
            } else {
                printf("Warning: Invalid count (%d) in doctor file.\n", readCount);
            }
        } else {
            printf("Warning: Could not read count from doctor file.\n");
        }
        fclose(fp);
    }

    // Load Appointments (similar logic)
    fp = fopen(APPOINTMENT_FILE, "rb");
    if (fp != NULL) {
        if(fread(&readCount, sizeof(int), 1, fp) == 1) {
            if (readCount >= 0 && readCount <= MAX_APPOINTMENTS) {
                int actualRead = fread(state->appointments, sizeof(struct Appointment), readCount, fp);
                state->appointmentCount = actualRead;
                if (actualRead != readCount) {
                     printf("Warning: Mismatch in expected (%d) and read (%d) appointment records.\n", readCount, actualRead);
                }
            } else if (readCount > MAX_APPOINTMENTS){
                printf("Warning: Appointment file contains more records (%d) than MAX_APPOINTMENTS (%d). Loading truncated list.\n", readCount, MAX_APPOINTMENTS);
                state->appointmentCount = fread(state->appointments, sizeof(struct Appointment), MAX_APPOINTMENTS, fp);
            } else {
                 printf("Warning: Invalid count (%d) in appointment file.\n", readCount);
            }
        } else {
             printf("Warning: Could not read count from appointment file.\n");
        }
        fclose(fp);
    }


     // Load Bills (similar logic)
    fp = fopen(BILL_FILE, "rb");
    if (fp != NULL) {
         if (fread(&readCount, sizeof(int), 1, fp) == 1) {
            if (readCount >= 0 && readCount <= MAX_BILLS) {
                int actualRead = fread(state->bills, sizeof(struct Bill), readCount, fp);
                state->billCount = actualRead;
                if (actualRead != readCount) {
                     printf("Warning: Mismatch in expected (%d) and read (%d) bill records.\n", readCount, actualRead);
                }
            } else if (readCount > MAX_BILLS) {
                printf("Warning: Bill file contains more records (%d) than MAX_BILLS (%d). Loading truncated list.\n", readCount, MAX_BILLS);
                state->billCount = fread(state->bills, sizeof(struct Bill), MAX_BILLS, fp);
            } else {
                 printf("Warning: Invalid count (%d) in bill file.\n", readCount);
            }
         } else {
             printf("Warning: Could not read count from bill file.\n");
         }
        fclose(fp);
    }

    // Optional: Add a message indicating data loading attempt
    // printf("Data loaded from files (if they existed).\n");
}

// --- Patient Management Functions (Operate on AppState) ---

int findPatientById(struct AppState* state, int id) {
    for (int i = 0; i < state->patientCount; i++) {
        if (state->patients[i].id == id) {
            return i; // Return index
        }
    }
    return -1; // Not found
}

// Helper to get patient name (no 'const')
char* getPatientNameById(struct AppState* state, int id) {
     int index = findPatientById(state, id);
     if (index != -1) {
         return state->patients[index].name;
     }
     // Return a modifiable string literal (use with caution, standard C doesn't guarantee writability)
     // A safer approach is a static buffer, but avoiding static globals too if possible.
     // Returning a simple string literal. Treat as read-only in calling code.
     return "Unknown Patient";
}


void addPatient(struct AppState* state) {
    if (state->patientCount >= MAX_PATIENTS) {
        printf("Maximum patient limit reached.\n");
        return;
    }

    struct Patient p; // Use 'struct Patient'
    p.id = state->nextPatientId++;

    printf("--- Add New Patient ---\n");
    getStringInput("Enter Name: ", p.name, NAME_LEN);
    p.age = getIntInput("Enter Age: ");
    getStringInput("Enter Gender: ", p.gender, GENDER_LEN);
    getStringInput("Enter Disease/Condition: ", p.disease, DISEASE_LEN);
    getStringInput("Enter Contact Number: ", p.contact, CONTACT_LEN);

    state->patients[state->patientCount++] = p;
    printf("Patient added successfully with ID: %d\n", p.id);
}

void viewPatients(struct AppState* state) {
    printf("\n--- Patient List (%d) ---\n", state->patientCount);
    if (state->patientCount == 0) {
        printf("No patients in the system.\n");
        return;
    }
    printf("-----------------------------------------------------------------------------------\n");
    printf("ID   | Name                 | Age | Gender   | Disease              | Contact       \n");
    printf("-----------------------------------------------------------------------------------\n");
    for (int i = 0; i < state->patientCount; i++) {
        printf("%-4d | %-20s | %-3d | %-8s | %-20s | %-13s\n",
               state->patients[i].id, state->patients[i].name, state->patients[i].age, state->patients[i].gender,
               state->patients[i].disease, state->patients[i].contact);
    }
    printf("-----------------------------------------------------------------------------------\n");
}

void editPatient(struct AppState* state) {
    int id = getIntInput("Enter Patient ID to edit: ");
    int index = findPatientById(state, id);

    if (index == -1) {
        printf("Patient with ID %d not found.\n", id);
        return;
    }

    struct Patient* p = &state->patients[index]; // Pointer for easier access

    printf("--- Editing Patient ID: %d ---\n", id);

    char tempBuffer[NAME_LEN]; // Temporary buffer for new input

    printf("Current Name: %s\n", p->name);
    getStringInput("Enter New Name (leave blank to keep current): ", tempBuffer, NAME_LEN);
    if (strlen(tempBuffer) > 0) {
        strcpy(p->name, tempBuffer);
    }

    printf("Current Age: %d\n", p->age);
    printf("Enter New Age (leave blank to keep current): ");
    fgets(tempBuffer, sizeof(tempBuffer), stdin);
    int newAge;
    if (tempBuffer[0] != '\n' && sscanf(tempBuffer, "%d", &newAge) == 1) {
         p->age = newAge;
    } // Else: Keep old age

    printf("Current Gender: %s\n", p->gender);
    getStringInput("Enter New Gender (leave blank to keep current): ", tempBuffer, GENDER_LEN);
     if (strlen(tempBuffer) > 0) {
        strcpy(p->gender, tempBuffer);
    }

    printf("Current Disease: %s\n", p->disease);
    getStringInput("Enter New Disease (leave blank to keep current): ", tempBuffer, DISEASE_LEN);
     if (strlen(tempBuffer) > 0) {
        strcpy(p->disease, tempBuffer);
    }

    printf("Current Contact: %s\n", p->contact);
    getStringInput("Enter New Contact (leave blank to keep current): ", tempBuffer, CONTACT_LEN);
     if (strlen(tempBuffer) > 0) {
        strcpy(p->contact, tempBuffer);
    }

    printf("Patient information updated successfully.\n");
}


void deletePatient(struct AppState* state) {
    int id = getIntInput("Enter Patient ID to delete: ");
    int index = findPatientById(state, id);

    if (index == -1) {
        printf("Patient with ID %d not found.\n", id);
        return;
    }

     // Confirmation
    char confirm[10];
    printf("Are you sure you want to delete patient '%s' (ID: %d)? (yes/no): ", state->patients[index].name, id);
    getStringInput("", confirm, sizeof(confirm)); // Prompt is blank
    if (strcmp(confirm, "yes") != 0) {
        printf("Deletion cancelled.\n");
        return;
    }

    // Shift elements to fill the gap
    for (int i = index; i < state->patientCount - 1; i++) {
        state->patients[i] = state->patients[i + 1];
    }
    state->patientCount--;

    printf("Patient with ID %d deleted successfully.\n", id);
}

// --- Doctor Management Functions (Operate on AppState) ---

int findDoctorById(struct AppState* state, int id) {
    for (int i = 0; i < state->doctorCount; i++) {
        if (state->doctors[i].id == id) {
            return i; // Return index
        }
    }
    return -1; // Not found
}

// Find doctor name by ID (no 'const')
char* getDoctorNameById(struct AppState* state, int id) {
     int index = findDoctorById(state, id);
     if (index != -1) {
         return state->doctors[index].name;
     }
     return "Unknown Doctor"; // Treat as read-only
}


void addDoctor(struct AppState* state) {
    if (state->doctorCount >= MAX_DOCTORS) {
        printf("Maximum doctor limit reached.\n");
        return;
    }

    struct Doctor d; // Use 'struct Doctor'
    d.id = state->nextDoctorId++;

    printf("--- Add New Doctor ---\n");
    getStringInput("Enter Name: ", d.name, NAME_LEN);
    getStringInput("Enter Specialization: ", d.specialization, SPECIALIZATION_LEN);
    getStringInput("Enter Availability (e.g., Mon-Fri 9am-5pm): ", d.availability, AVAILABILITY_LEN);

    state->doctors[state->doctorCount++] = d;
    printf("Doctor added successfully with ID: %d\n", d.id);
}

void viewDoctors(struct AppState* state) {
    printf("\n--- Doctor List (%d) ---\n", state->doctorCount);
    if (state->doctorCount == 0) {
        printf("No doctors in the system.\n");
        return;
    }
    printf("-------------------------------------------------------------------------------------\n");
    printf("ID   | Name                 | Specialization       | Availability                    \n");
    printf("-------------------------------------------------------------------------------------\n");
    for (int i = 0; i < state->doctorCount; i++) {
        printf("%-4d | %-20s | %-20s | %-30s\n",
               state->doctors[i].id, state->doctors[i].name, state->doctors[i].specialization, state->doctors[i].availability);
    }
    printf("-------------------------------------------------------------------------------------\n");
}

void searchDoctor(struct AppState* state) {
    char query[NAME_LEN];
    int found = 0;
    printf("--- Search Doctor ---\n");
    getStringInput("Enter Doctor Name or Specialization to search: ", query, NAME_LEN);

    printf("\n--- Search Results ---\n");
    printf("-------------------------------------------------------------------------------------\n");
    printf("ID   | Name                 | Specialization       | Availability                    \n");
    printf("-------------------------------------------------------------------------------------\n");

    for (int i = 0; i < state->doctorCount; i++) {
        // Simple substring search (case-sensitive)
        if (strstr(state->doctors[i].name, query) != NULL || strstr(state->doctors[i].specialization, query) != NULL) {
            printf("%-4d | %-20s | %-20s | %-30s\n",
                   state->doctors[i].id, state->doctors[i].name, state->doctors[i].specialization, state->doctors[i].availability);
            found++;
        }
    }
    printf("-------------------------------------------------------------------------------------\n");

    if (found == 0) {
        printf("No doctors found matching '%s'.\n", query);
    } else {
        printf("%d doctor(s) found.\n", found);
    }
}

// --- Appointment Management Functions (Operate on AppState) ---

int findAppointmentById(struct AppState* state, int id) {
    for (int i = 0; i < state->appointmentCount; i++) {
        if (state->appointments[i].id == id) {
            return i; // Return index
        }
    }
    return -1; // Not found
}

void scheduleAppointment(struct AppState* state) {
    if (state->appointmentCount >= MAX_APPOINTMENTS) {
        printf("Maximum appointment limit reached.\n");
        return;
    }
    if (state->patientCount == 0) {
        printf("No patients in the system. Please add a patient first.\n");
        return;
    }
     if (state->doctorCount == 0) {
        printf("No doctors in the system. Please add a doctor first.\n");
        return;
    }

    struct Appointment appt; // Use 'struct Appointment'
    appt.id = state->nextAppointmentId++;
    int patientId, doctorId;
    int patientIndex, doctorIndex;

    printf("--- Schedule New Appointment ---\n");

    // Get and validate Patient ID
    viewPatients(state); // Show available patients
    while (1) {
        patientId = getIntInput("Enter Patient ID: ");
        patientIndex = findPatientById(state, patientId);
        if (patientIndex != -1) {
            appt.patientId = patientId;
            break;
        } else {
            printf("Invalid Patient ID. Please try again.\n");
        }
    }

    // Get and validate Doctor ID
    viewDoctors(state); // Show available doctors
     while (1) {
        doctorId = getIntInput("Enter Doctor ID: ");
        doctorIndex = findDoctorById(state, doctorId);
        if (doctorIndex != -1) {
            appt.doctorId = doctorId;
            break;
        } else {
            printf("Invalid Doctor ID. Please try again.\n");
        }
    }

    // Get Date and Time
    getStringInput("Enter Appointment Date (YYYY-MM-DD): ", appt.date, DATE_LEN);
    getStringInput("Enter Appointment Time (HH:MM): ", appt.time, TIME_LEN);

    state->appointments[state->appointmentCount++] = appt;
    printf("Appointment scheduled successfully for Patient %s with Dr. %s on %s at %s (Appt ID: %d)\n",
           state->patients[patientIndex].name, state->doctors[doctorIndex].name, appt.date, appt.time, appt.id);
}

void viewAppointments(struct AppState* state) {
    printf("\n--- Scheduled Appointments (%d) ---\n", state->appointmentCount);
    if (state->appointmentCount == 0) {
        printf("No appointments scheduled.\n");
        return;
    }
    printf("-------------------------------------------------------------------------------------------\n");
    printf("Appt ID | Patient ID | Patient Name       | Doctor ID | Doctor Name        | Date       | Time  \n");
    printf("-------------------------------------------------------------------------------------------\n");

    for (int i = 0; i < state->appointmentCount; i++) {
        char* patientName = getPatientNameById(state, state->appointments[i].patientId);
        char* doctorName = getDoctorNameById(state, state->appointments[i].doctorId);

        printf("%-7d | %-10d | %-18s | %-9d | %-18s | %-10s | %-5s\n",
               state->appointments[i].id,
               state->appointments[i].patientId, patientName,
               state->appointments[i].doctorId, doctorName,
               state->appointments[i].date, state->appointments[i].time);
    }
    printf("-------------------------------------------------------------------------------------------\n");
}

void cancelAppointment(struct AppState* state) {
    int id = getIntInput("Enter Appointment ID to cancel: ");
    int index = findAppointmentById(state, id);

    if (index == -1) {
        printf("Appointment with ID %d not found.\n", id);
        return;
    }

     // Confirmation
    char confirm[10];
    printf("Are you sure you want to cancel appointment ID %d? (yes/no): ", id);
    getStringInput("", confirm, sizeof(confirm)); // Blank prompt
     if (strcmp(confirm, "yes") != 0) {
        printf("Cancellation aborted.\n");
        return;
    }

    // Shift elements to fill the gap
    for (int i = index; i < state->appointmentCount - 1; i++) {
        state->appointments[i] = state->appointments[i + 1];
    }
    state->appointmentCount--;

    printf("Appointment with ID %d cancelled successfully.\n", id);
}

// --- Billing Functions (Operate on AppState) ---

int findBillById(struct AppState* state, int id) {
    for (int i = 0; i < state->billCount; i++) {
        if (state->bills[i].id == id) {
            return i; // Return index
        }
    }
    return -1; // Not found
}


void generateBill(struct AppState* state) {
    if (state->billCount >= MAX_BILLS) {
        printf("Maximum bill limit reached.\n");
        return;
    }
     if (state->patientCount == 0) {
        printf("No patients in the system to bill.\n");
        return;
    }

    struct Bill b; // Use 'struct Bill'
    b.id = state->nextBillId++;
    int patientId, doctorId = -1;
    int patientIndex, doctorIndex = -1;

    printf("--- Generate New Bill ---\n");

    // Get and validate Patient ID
    viewPatients(state);
    while (1) {
        patientId = getIntInput("Enter Patient ID for the bill: ");
        patientIndex = findPatientById(state, patientId);
        if (patientIndex != -1) {
            b.patientId = patientId;
            break;
        } else {
            printf("Invalid Patient ID. Please try again.\n");
        }
    }

    // Optionally, link to a specific doctor for the fee
    char linkDoctor[5];
    printf("Is this bill related to a specific doctor consultation? (yes/no): ");
    getStringInput("", linkDoctor, sizeof(linkDoctor)); // Blank prompt
    if(strcmp(linkDoctor, "yes") == 0 && state->doctorCount > 0) { // Check if doctors exist
        viewDoctors(state);
         while (1) {
            doctorId = getIntInput("Enter Doctor ID for the consultation fee: ");
            doctorIndex = findDoctorById(state, doctorId);
            if (doctorIndex != -1) {
                b.doctorId = doctorId;
                break;
            } else {
                 printf("Invalid Doctor ID. Please try again.\n");
            }
        }
         b.doctorFee = getFloatInput("Enter Doctor Consultation Fee: ");
    } else {
        if(strcmp(linkDoctor, "yes") == 0 && state->doctorCount == 0) {
            printf("Cannot link doctor fee, no doctors in the system.\n");
        }
        b.doctorId = -1; // Indicate no specific doctor linked or fee from doctor
        b.doctorFee = 0.0;
    }

    // Get Bill Date
    getStringInput("Enter Bill Date (YYYY-MM-DD): ", b.dateGenerated, DATE_LEN);

    // Calculate Total
    b.totalAmount = b.doctorFee; // Add other costs if implemented

    state->bills[state->billCount++] = b;
    printf("Bill generated successfully for Patient %s (Bill ID: %d)\n",
           state->patients[patientIndex].name, b.id);
    printf("Total Amount: %.2f\n", b.totalAmount);
}

void printInvoice(struct AppState* state) {
    int billId = getIntInput("Enter Bill ID to print invoice: ");
    int billIndex = findBillById(state, billId);

    if (billIndex == -1) {
        printf("Bill with ID %d not found.\n", billId);
        return;
    }

    struct Bill b = state->bills[billIndex];
    int patientIndex = findPatientById(state, b.patientId);

    if (patientIndex == -1) {
        printf("Error: Patient associated with this bill (ID: %d) not found!\n", b.patientId);
        return; // Cannot print invoice without patient details
    }
    struct Patient p = state->patients[patientIndex];
    char* doctorName = (b.doctorId != -1) ? getDoctorNameById(state, b.doctorId) : "N/A";


    printf("\n\n--- HOSPITAL INVOICE ---\n");
    printf("----------------------------------------\n");
    printf(" Bill ID       : %d\n", b.id);
    printf(" Bill Date     : %s\n", b.dateGenerated);
    printf("----------------------------------------\n");
    printf(" Patient Details:\n");
    printf("   Patient ID  : %d\n", p.id);
    printf("   Name        : %s\n", p.name);
    printf("   Age         : %d\n", p.age);
    printf("   Gender      : %s\n", p.gender);
    printf("   Contact     : %s\n", p.contact);
    printf("----------------------------------------\n");
    printf(" Charges:\n");
    if (b.doctorId != -1) {
       printf("   Doctor Fee (Dr. %s): %.2f\n", doctorName, b.doctorFee);
    }
    // Add lines for medicine, tests etc. if implemented
    printf("----------------------------------------\n");
    printf(" Total Amount  : %.2f\n", b.totalAmount);
    printf("----------------------------------------\n");
    printf(" Thank you!\n");
    printf("----------------------------------------\n\n");
}

void viewBills(struct AppState* state) {
    printf("\n--- Bill List (%d) ---\n", state->billCount);
    if (state->billCount == 0) {
        printf("No bills generated yet.\n");
        return;
    }
    printf("-----------------------------------------------------------------------------\n");
    printf("Bill ID | Patient ID | Patient Name       | Doctor Fee | Total Amount | Date \n");
    printf("-----------------------------------------------------------------------------\n");
    for (int i = 0; i < state->billCount; i++) {
         char* patientName = getPatientNameById(state, state->bills[i].patientId);
        printf("%-7d | %-10d | %-18s | %-10.2f | %-12.2f | %-10s\n",
               state->bills[i].id,
               state->bills[i].patientId,
               patientName,
               state->bills[i].doctorFee,
               state->bills[i].totalAmount,
               state->bills[i].dateGenerated);
    }
     printf("-----------------------------------------------------------------------------\n");
}


// --- Menu Functions (Now require AppState pointer) ---

void patientMenu(struct AppState* state) {
    int choice;
    while (1) {
        printf("\n--- Patient Management ---\n");
        printf("1. Add New Patient\n");
        printf("2. View All Patients\n");
        printf("3. Edit Patient Information\n");
        printf("4. Delete Patient Record\n");
        printf("0. Back to Main Menu\n");
        choice = getIntInput("Enter your choice: ");

        switch (choice) {
            case 1: addPatient(state); break;
            case 2: viewPatients(state); break;
            case 3: editPatient(state); break;
            case 4: deletePatient(state); break;
            case 0: return;
            default: printf("Invalid choice. Please try again.\n");
        }
    }
}

void doctorMenu(struct AppState* state) {
    int choice;
    while (1) {
        printf("\n--- Doctor Management ---\n");
        printf("1. Add New Doctor\n");
        printf("2. View All Doctors\n");
        printf("3. Search Doctor (by Name/Specialization)\n");
        printf("0. Back to Main Menu\n");
        choice = getIntInput("Enter your choice: ");

        switch (choice) {
            case 1: addDoctor(state); break;
            case 2: viewDoctors(state); break;
            case 3: searchDoctor(state); break;
            case 0: return;
            default: printf("Invalid choice. Please try again.\n");
        }
    }
}

void appointmentMenu(struct AppState* state) {
    int choice;
    while (1) {
        printf("\n--- Appointment Management ---\n");
        printf("1. Schedule New Appointment\n");
        printf("2. View All Appointments\n");
        printf("3. Cancel Appointment\n");
        printf("0. Back to Main Menu\n");
        choice = getIntInput("Enter your choice: ");

        switch (choice) {
            case 1: scheduleAppointment(state); break;
            case 2: viewAppointments(state); break;
            case 3: cancelAppointment(state); break;
            case 0: return;
            default: printf("Invalid choice. Please try again.\n");
        }
    }
}

void billingMenu(struct AppState* state) {
     int choice;
    while (1) {
        printf("\n--- Billing Management ---\n");
        printf("1. Generate New Bill\n");
        printf("2. View All Bills\n");
        printf("3. Print Invoice\n");
        printf("0. Back to Main Menu\n");
        choice = getIntInput("Enter your choice: ");

        switch (choice) {
            case 1: generateBill(state); break;
            case 2: viewBills(state); break;
            case 3: printInvoice(state); break;
            case 0: return;
            default: printf("Invalid choice. Please try again.\n");
        }
    }
}

// --- Main Function ---
int main() {
    // Declare the application state structure
    struct AppState appState;

    // Initialize counts (important before loading)
    appState.patientCount = 0;
    appState.doctorCount = 0;
    appState.appointmentCount = 0;
    appState.billCount = 0;
    // Initial ID values will be set by loadCounters

    loadData(&appState); // Load existing data into the state structure

    int choice;
    while (1) {
        printf("\n===== Hospital Management System =====\n");
        printf("1. Patient Management\n");
        printf("2. Doctor Management\n");
        printf("3. Appointment Management\n");
        printf("4. Billing System\n");
        printf("5. Save Data to Files\n");
        printf("0. Exit\n");
        printf("======================================\n");
        choice = getIntInput("Enter your choice: ");

        switch (choice) {
            // Pass the address of the state structure to menu functions
            case 1: patientMenu(&appState); break;
            case 2: doctorMenu(&appState); break;
            case 3: appointmentMenu(&appState); break;
            case 4: billingMenu(&appState); break;
            case 5: saveData(&appState); break;
            case 0:
                printf("Exiting program. Do you want to save data first? (yes/no): ");
                char saveChoice[5];
                getStringInput("", saveChoice, sizeof(saveChoice)); // Blank prompt
                if (strcmp(saveChoice, "yes") == 0) {
                    saveData(&appState);
                }
                printf("Goodbye!\n");
                return 0; // Exit program
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0; // Should not reach here
}