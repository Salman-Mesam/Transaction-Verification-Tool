//Mohammad Salman Mesam
//260860161


#include <assert.h>
#include <memory.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct ACCOUNT {
  int accountNumber;
  double startingBalance;
  double endingBalance;
  struct ACCOUNT *next;
};

int main(int argc, char *argv[]) {
  // check if args exist and valid
  if (argc < 3) {
    printf("Wrong number of arguments!\n"
           "./tv STATE TRANSACTIONS\n"
           "./tv state.csv transaction.csv\n"
           "./tv ../state.csv /data/log/transaction.csv\n");
    return 1;
  }

  FILE *state = fopen(argv[1], "r");
  if (!state) {
    printf("Unable to open filename %s\n"
           "./tv STATE TRANSACTIONS\n"
           "./tv state.csv transaction.csv\n"
           "./tv ../state.csv /data/log/transaction.csv\n",
           argv[1]);
    return 2;
  }

  FILE *transaction = fopen(argv[2], "r");
  if (!transaction) {
    printf("Unable to open filename %s\n"
           "./tv STATE TRANSACTIONS\n"
           "./tv state.csv transaction.csv\n"
           "./tv ../state.csv /data/log/transaction.csv\n",
           argv[2]);
    return 2;
  }

  char line[2000];
  memset(line, 0, sizeof(line));

  struct ACCOUNT *head = NULL;

  fgets(line, sizeof(line), state);
  while (fgets(line, sizeof(line), state) != NULL) {
    struct ACCOUNT *account = malloc(sizeof(*account));
    account->next = NULL;
    if (sscanf(line, "%d,%lf,%lf", &account->accountNumber,
               &account->startingBalance, &account->endingBalance) < 3) {
      free(account);
      continue;
    }

    if (head) {
      struct ACCOUNT *current = head;
      struct ACCOUNT *prev = NULL;
      for (;;) {
        if (current) {
          // during iteration
          if (account->accountNumber == current->accountNumber) {
            printf("Duplicate account number [account, start, end]: %d %f %f\n",
                   account->accountNumber, account->startingBalance,
                   account->endingBalance);
            free(account);
            break;
          } else if (account->accountNumber < current->accountNumber) {
            account->next = current;
            if (prev) {
              prev->next = account;
            } else {
              head = account;
            }
            break;
          }
          prev = current;
          current = current->next;
        } else {
          // exhausted iteration
          if (prev) {
            prev->next = account;
            break;
          } else {
            head = account;
            break;
          }
        }
      }
    } else {
      head = account;
    }
  }

  fgets(line, sizeof(line), transaction);
  while (fgets(line, sizeof(line), transaction) != NULL) {
    int accountNumber;
    char mode;
    double absoluteValueAmount;

    if (sscanf(line, "%d,%c,%lf", &accountNumber, &mode, &absoluteValueAmount) <
        3) {
      continue;
    }

    if (!head) {
      printf("File state.csv is empty. Unable to validate transaction.csv.\n");
      return 3;
    }

    struct ACCOUNT *account = head;
    while (account) {
      if (account->accountNumber == accountNumber) {
        break;
      } else {
        account = account->next;
      }
    }

    if (!account) {
      printf("Account not found (account, mode, amount): %d %c %f\n",
             accountNumber, mode, absoluteValueAmount);
      continue;
    }

    switch (mode) {
    case 'd': {
      account->startingBalance += absoluteValueAmount;
      break;
    }
    case 'w': {
      const double startingBalance = account->startingBalance;
      account->startingBalance -= absoluteValueAmount;
      if (account->startingBalance < 0) {
        printf("Balance below zero error (account, mode, transaction, "
               "startingBalance before): %d %c %f %f\n",
               accountNumber, mode, absoluteValueAmount, startingBalance);
        account->startingBalance = 0;
      }
      break;
    }
    }
  }

  while (head) {
    struct ACCOUNT *const tmp = head;
    head = head->next;
    if (tmp->startingBalance != tmp->endingBalance) {
      printf("End of day balances do not agree (account, starting, ending): %d "
             "%f %f\n",
             tmp->accountNumber, tmp->startingBalance, tmp->endingBalance);
    }
    free(tmp);
  }

  return 0;
}
