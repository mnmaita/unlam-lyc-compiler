#ifndef INCLUDE_HEADER_H_
#define INCLUDE_HEADER_H_

#include "list.h"

/* Special functions data structures */
List EqualsList;
List AllEqualsStack;
List AllEqualsExpressions;
/* Special functions data indexes and total expression count */
int TotalExpressionAllEquals = 0;
int TotalExpressionLists = 0;
int AllequalIndex = 0;
int EqualsIndex = 0;
/* Other Flags */
int AssignmentType = 0;
int IsIfCondition = 0;

#endif // INCLUDE_HEADER_H_
