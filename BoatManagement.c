#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NAME 127
#define MAX_SIZE 120
#define MAX_FEET 100
#define TRAILOR_TAG_LEN 7

typedef enum {
    SLIP,
    LAND,
    TRAILOR,
    STORAGE
} PlaceType;

typedef union {
    int slipNumber;
    char bayLetter;
    char trailorLicenseTag[TRAILOR_TAG_LEN];
    int storageSpaceNumber;
} PlaceTypeInfo;

typedef struct {
    char name[MAX_NAME]; //127 
    int length; //100 ft max
    PlaceType place;
    PlaceTypeInfo info;
    float owed;
} Boat;


/**
The user might enter a non-existent boat name for removal and payment, 
or an amount too large for payment, in which case the action is not completed.
 */
void acceptPayment(Boat* inventory, int n) {
    char name[MAX_NAME];
    float payment;

    printf("Enter boat name: ");
    fgets(name, MAX_NAME, stdin);
    name[strcspn(name, "\n")] = 0; // remove newline character

    int found = 0;
    for (int i = 0; i < n; i++) {
        if (strcasecmp(inventory[i].name, name) == 0) {
            found = 1;
            printf("Boat found!\n");
            printf("Amount owed: $%.2f\n", inventory[i].owed);

            printf("Enter payment amount: ");
            scanf("%f", &payment);
            getchar(); // remove newline character from buffer

            // QA: Can we assume a user would try to do enter a negative value?
            if (payment < 0) {
                printf("Error: payment cannot be negative.\n");
                break;
            }

            if (payment > inventory[i].owed) {
                printf("Error: payment exceeds amount owed.\n");
                break;
            } else {
                inventory[i].owed -= payment;
                printf("New amount owed: $%.2f\n", inventory[i].owed);
                break;
            }
            break;
        }
    }

    if (!found) {
        printf("Error: boat not found.\n");
    }
}

/**
Print the boat inventory, sorted alphabetically by boat name 
(hint: keep the array packed and sorted by boat name)

A negative value if b1->name is less than b2->name
Zero if they are equal
A positive value if b1->name is greater than b2->name
 */

// int compare(const void *a, const void *b);
int compareBoats(const void* boat1, const void* boat2) {
    // QA: Can the pointers be null?
    const Boat* b1 = (const Boat*)boat1;
    const Boat* b2 = (const Boat*)boat2;
    return strcasecmp(b1->name, b2->name);
}

// int compareBoats(const Boat* boat1, const Boat* boat2) {
//     return strcasecmp(boat1->name, boat2->name);
// }


void BoatInventorySorted(Boat* inventory, int n) {
    qsort(inventory, n, sizeof(Boat), compareBoats);
    printf("\nBoat inventory (sorted):\n");
    for (int i = 0; i < n; i++) {
        printf("%s, %d, ", inventory[i].name, inventory[i].length);
        switch (inventory[i].place) {
            case SLIP:
                printf("SLIP, %d", inventory[i].info.slipNumber);
                break;
            case LAND:
                printf("LAND, %c", inventory[i].info.bayLetter);
                break;
            case TRAILOR:
                printf("TRAILOR, %s", inventory[i].info.trailorLicenseTag);
                break;
            case STORAGE:
                printf("STORAGE, %d", inventory[i].info.storageSpaceNumber);
                break;
            default:
                fprintf(stderr, "Error: invalid place type.\n");
            exit(EXIT_FAILURE);
        }
        printf(", %.2f\n", inventory[i].owed);
    }
}

//*inventoryPtr dereferences inventoryPtr to access the actual inventory of boats.
//*nPtr dereferences nPtr to access and modify the actual number of boats.
//

void removeBoatByName(Boat** inventoryPtr, int* nPtr, char* name) {
    Boat* inventory = *inventoryPtr;
    int n = *nPtr;
    int index = -1;

    for (int i = 0; i < n; i++) {
        // Deal w/ cases
        if (strcasecmp(inventory[i].name, name) == 0) {
            index = i;
            break;
        }
    }
    
    // Copies all the boats to new list, old list is deleted
    // Free the space of the old list
    // The total number of boats is reduced by one.

    if (index != -1) {
        if (n == 1) {
            // If it's the last boat, free inventory and set to NULL
            free(inventory);
            *inventoryPtr = NULL;
            *nPtr = 0;
        } else {
            // Allocate new memory for a smaller array
            Boat* tmp = (Boat*)malloc((n - 1) * sizeof(Boat));
            if (tmp == NULL) {
                // Test malloc errors
                fprintf(stderr, "Error: memory allocation failed.\n");
                exit(EXIT_FAILURE);
            }
            
            // Copy all boats except the one to remove
            for (int i = 0; i < index; i++) {
                tmp[i] = inventory[i];
            }
            for (int i = index + 1; i < n; i++) {
                tmp[i - 1] = inventory[i];
            }

            free(inventory);
            *inventoryPtr = tmp;
            *nPtr = n - 1;
        }
        printf("Boat %s removed from inventory.\n", name);
    } else {
        printf("Boat %s not found in inventory.\n", name);
    }
}

// i.e Big Brother             20'       slip   # 27   Owes $1200.00
// TODO: add ''' for length , # for Place Info in case of SLIP or STORAGE

void PrintInventory(Boat* inventory, int n) {
    printf("%-15s %-8s %-15s %-15s %s\n", "Name", "Length", "Place Type", "Place Info", "Amount Owed");
    for (int i = 0; i < n; i++) {
        printf("%-15s %-8d ", inventory[i].name, inventory[i].length);

        switch (inventory[i].place) {
            case SLIP:
                printf("%-15s %d", "SLIP", inventory[i].info.slipNumber);
                break;
            case LAND:
                printf("%-15s %c", "LAND", inventory[i].info.bayLetter);
                break;
            case TRAILOR:
                printf("%-15s %s", "TRAILOR", inventory[i].info.trailorLicenseTag);
                break;
            case STORAGE:
                printf("%-15s %d", "STORAGE", inventory[i].info.storageSpaceNumber);
                break;
            default:
                fprintf(stderr, "Error: invalid place type.\n");
                exit(EXIT_FAILURE);
        }

        printf(" %-15.2f\n", inventory[i].owed);
    }
}

void AddBoat(Boat** inventory, int* n) {
    char input[MAX_NAME + MAX_SIZE + 10]; // Assuming input format: name,length,place,info,owed

    printf("Enter boat information in format: \nname,length,place,info,owed\n");
    printf("Enter 'done' when finished.\n\n");

    while (1) {
        printf("Enter boat %d: ", *n + 1);
        fgets(input, sizeof(input), stdin);

        if (strncmp(input, "done", 4) == 0) {
            break;
        }

        // Check for empty input
        if (strlen(input) <= 1) {
            printf("Error: empty input, please try again.\n");
            continue;
        }

        // Allocate memory for every 10 new boat to optimize instead of every single new entry 
        // QA: Should I only allocate memory for every ten or for every single new entry
        Boat* temp = realloc(*inventory, (*n + 10) * sizeof(Boat));
        if (temp == NULL) {
            printf("Error: could not allocate memory for new boat\n");
            return;
        }
        *inventory = temp;

        (*n)++;  // Increment boat count after successful memory allocation

        // Name parsing input
        char* token = strtok(input, ",");
        if (token == NULL) {
            fprintf(stderr, "Error: missing boat name.\n");
            (*n)--;  // Cancels the addition
            continue;
        }
        // Copy boat name into the name field of the last boat in the inventory
        strncpy((*inventory)[*n-1].name, token, MAX_NAME);
        // Tell where the string stops
        (*inventory)[*n-1].name[MAX_NAME-1] = '\0';

        // Length parsing, grab next token from where it left off
        token = strtok(NULL, ",");
        // atoi, Converts string form to integer value
        if (token == NULL || ((*inventory)[*n-1].length = atoi(token)) <= 0) {
            fprintf(stderr, "Error: invalid length value.\n");
            (*n)--;  // Cancels the addition
            continue;
        }

        // Place parsing
        token = strtok(NULL, ",");
        if (token == NULL) {
            fprintf(stderr, "Error: missing place type.\n");
            (*n)--;  // Cancels the addition
            continue;
        }
        // QA: Should I change everything to strcasecmp or is it fine that I changed the token to upper
        token[strcspn(token, "\n")] = '\0';  // Remove trailing newline
        for (int i = 0; token[i]; i++) {
            token[i] = toupper(token[i]);
        }

        if (strcasecmp(token, "SLIP") == 0) {
            (*inventory)[*n-1].place = SLIP;
        } else if (strcasecmp(token, "LAND") == 0) {
            (*inventory)[*n-1].place = LAND;
        } else if (strcasecmp(token, "TRAILOR") == 0) {
            (*inventory)[*n-1].place = TRAILOR;
        } else if (strcasecmp(token, "STORAGE") == 0) {
            (*inventory)[*n-1].place = STORAGE;
        } else {
            fprintf(stderr, "Error: invalid place type '%s'.\n", token);
            (*n)--;  // Cancels the addition
            continue;
        }

        // Info parsing
        token = strtok(NULL, ",");
        if (token == NULL) {
            fprintf(stderr, "Error: missing place info.\n");
            (*n)--;  // Cancels the addition
            continue;
        }
        switch ((*inventory)[*n-1].place) {
            case SLIP:
                (*inventory)[*n-1].info.slipNumber = atoi(token);
                break;
            case LAND:
                (*inventory)[*n-1].info.bayLetter = toupper(*token);
                break;
            case TRAILOR:
                strncpy((*inventory)[*n-1].info.trailorLicenseTag, token, TRAILOR_TAG_LEN - 1);
                (*inventory)[*n-1].info.trailorLicenseTag[TRAILOR_TAG_LEN - 1] = '\0';
                break;
            case STORAGE:
                (*inventory)[*n-1].info.storageSpaceNumber = atoi(token);
                break;
        }

        // Owed parsing
        token = strtok(NULL, ",");
        if (token == NULL) {
            fprintf(stderr, "Error: missing owed value.\n");
            (*n)--;  // Cancels the addition
            continue;
        }
        // atof, Converts string form to float value
        (*inventory)[*n-1].owed = atof(token);
    }
}



void updateAmountDueMonth(Boat* inventory, int n) {
    // QA: Should I change these to constants and make them global variables
    float slipAmount = 12.5;
    float landAmount = 14.0;
    float trailorAmount = 25.0;
    float storageAmount = 11.2;

    for (int i = 0; i < n; i++) {
        switch (inventory[i].place) {
            case SLIP:
                inventory[i].owed += inventory[i].length * slipAmount;
                break;
            case LAND:
                inventory[i].owed += inventory[i].length * landAmount;
                break;
            case TRAILOR:
                inventory[i].owed += inventory[i].length * trailorAmount;
                break;
            case STORAGE:
                inventory[i].owed += inventory[i].length * storageAmount;
                break;
            default:
                fprintf(stderr, "Error: invalid place type.\n");
                exit(EXIT_FAILURE);
        }
    }
}

// BoatData.csv
/**
Boat data in .csv format will be correctly formatted and within limits
Numeric input will be syntactically correct
 */

void writeBoatDataToCSV(char* file_name, Boat* inventory, int n) {
    FILE* file = fopen(file_name, "w");
    if (file == NULL) {
        printf("Error: could not open file %s for writing\n", file_name);
        return;
    }

    // Write header row
    fprintf(file, "Name,Length,Place,Info,Owed\n");

    // Write data rows
    for (int i = 0; i < n; i++) {
        Boat* boat = &inventory[i];

        // Validate boat information before writing to file
        if (strlen(boat->name) == 0) {
            printf("Skipping boat with invalid name at index %d\n", i);
            continue;
        }
        if (boat->length <= 0 || boat->length > MAX_FEET) {
            printf("Skipping boat with invalid length at index %d\n", i);
            continue;
        }
        if (boat->owed < 0) {
            printf("Skipping boat with negative owed amount at index %d\n", i);
            continue;
        }

        // Write name and length
        fprintf(file, "%s,%d,", boat->name, boat->length);

        // Write place type and place info based on the type
        switch (boat->place) {
            case SLIP:
                if (boat->info.slipNumber <= 0) {
                    printf("Skipping boat with invalid slip number at index %d\n", i);
                    continue;
                }
                // Keep structure with commas, strtok ignores
                fprintf(file, "SLIP,,,,%d,", boat->info.slipNumber);
                break;
            case LAND:
                if (!isalpha(boat->info.bayLetter)) {
                    printf("Skipping boat with invalid bay letter at index %d\n", i);
                    continue;
                }
                fprintf(file, "LAND,,,,%c,", boat->info.bayLetter);
                break;
            case TRAILOR:
                if (strlen(boat->info.trailorLicenseTag) != TRAILOR_TAG_LEN) {
                    printf("Skipping boat with invalid trailor license tag at index %d\n", i);
                    continue;
                }
                fprintf(file, "TRAILOR,,,,%s,", boat->info.trailorLicenseTag);
                break;
            case STORAGE:
                if (boat->info.storageSpaceNumber <= 0) {
                    printf("Skipping boat with invalid storage space number at index %d\n", i);
                    continue;
                }
                fprintf(file, "STORAGE,,,,%d,", boat->info.storageSpaceNumber);
                break;
            default:
                printf("Skipping boat with invalid place type at index %d\n", i);
                continue;
        }

        // Write owed amount
        fprintf(file, "%.2f\n", boat->owed);
    }

    fclose(file);
    printf("Boat data successfully written to %s\n", file_name);
}


/**
Once the boat data is loaded the program must offer a menu of options:

Print the boat inventory, sorted alphabetically by boat name (hint: keep the array packed and sorted by boat name)
Add a boat by providing as a string that looks like one line of the .csv file
Remove a boat by name
Accept a payment for the boat, up to the amount owed
Update the amount owed because a new month has started
Exit

The program must be user friendly, but you can assume that the user will enter reasonable data values:

 */

int main(int argc, char *argv[]) {
    // Check if user provided a file name, if they didn't teach them
    // Check number of command line args	
    if (argc < 2) {
        printf("Usage: %s file_name\n", argv[0]);
        return 1;
    }

    char* file_name = argv[1];
    FILE* file = fopen(file_name, "r");
    
    // Test file open/close errors
    if (file == NULL) {
        printf("Error: could not open file %s\n", file_name);
        return 1;
    }

    Boat* inventory = malloc(sizeof(Boat) * MAX_SIZE);;
    if (inventory == NULL) {
        printf("Error: could not allocate memory for inventory\n");
        return 1;
    }

    // Welcome message
    printf("Welcome to the Boat Management System\n");
    printf("-------------------------------------\n");

    int n = 0;
    char line[MAX_NAME*4]; // max line length of CSV file
    while (fgets(line, sizeof(line), file) != NULL) {
        char* name = strtok(line, ",");
        int length = atoi(strtok(NULL, ","));
        char* place_str = strtok(NULL, ",");
        PlaceType place;
        if (strcasecmp(place_str, "SLIP") == 0) {
            place = SLIP;
            inventory[n].info.slipNumber = atoi(strtok(NULL, ","));
        } else if (strcasecmp(place_str, "LAND") == 0) {
            place = LAND;
            inventory[n].info.bayLetter = *strtok(NULL, ",");
        } else if (strcasecmp(place_str, "TRAILOR") == 0) {
            place = TRAILOR;
            strcpy(inventory[n].info.trailorLicenseTag, strtok(NULL, ","));
        } else if (strcasecmp(place_str, "STORAGE") == 0) {
            place = STORAGE;
            inventory[n].info.storageSpaceNumber = atoi(strtok(NULL, ","));
        } else {
            printf("Error: invalid place for boat %s\n", name);
            continue;
        }
        float owed = atof(strtok(NULL, ","));
        strcpy(inventory[n].name, name);
        inventory[n].length = length;
        inventory[n].place = place;
        inventory[n].owed = owed;
        n++;
    }

    Boat* boat_inventory = inventory;

    while (1) {
        printf("(I)nventory, (A)dd, (R)emove, (P)ayment, (M)onth, e(X)it : ");
        printf("%s",file_name);
        char input = getchar();
        while (getchar() != '\n'); // clear input buffer

        switch (tolower(input)) {
            case 'i':
                // print inventory
                BoatInventorySorted(boat_inventory, n);
                fclose(file);
                break;
            case 'a':
                // add boat
                AddBoat(&boat_inventory, &n);
                break;
            case 'r':
                // remove boat
                printf("Enter the name of the boat to remove: ");
                char name[MAX_NAME];
                fgets(name, sizeof(name), stdin);
                name[strcspn(name, "\n")] = '\0'; // remove trailing newline character

                removeBoatByName(&boat_inventory, &n, name);
                break;
            case 'p':
                // payment
                acceptPayment(boat_inventory, n);
                break;
            case 'm':
                // month
                updateAmountDueMonth(boat_inventory, n);
                break;
            case 'x':
                // exit
                printf("Exiting the Boat Management System");
                // Free all memory on exit
                free(inventory);
                // QA: Close the file just in case it's open
                fclose(file);
                return EXIT_SUCCESS;
            default:
                printf("Invalid option.\n");
        }
    }
    return EXIT_SUCCESS;
}
