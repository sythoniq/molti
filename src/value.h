#ifndef MOLTI_VALUE_H
#define MOLTI_VALUE_H

typedef double Value;

typedef struct {
				int capacity;
				int count;
				Value *values;
} ValueArray;

typedef enum {
	VAL_BOOL,
	VAL_NIL,
	VAL_NUMBER
} ValueType;

typedef struct {
	ValueType type;
	union {
		bool boolean;
		double number;
	} as;
} Value;

#define IS_BOOL(value)    ((value).type == VAL_BOOL)
#define IS_NIL(value)     ((value).type == VAL_NIL)
#define IS_NUMBER(value)  ((value).type == VAL_NUMBER)

#define AS_BOOL(value)    ((value).as.boolean)
#define AS_NUMBER(value)  ((value).as.number)

#define BOOL_VAL(value)   ((Value){VAL_BOOL, {.boolean = value}})
#define NIL_VAL           ((Value){VAL_NIL, {.number = 0}})
#define NUMBER_VAL(value) ((Value){VAL_NUMBER, {.number = value}})

void initValueArray(ValueArray *arr);
void writeValueArray(ValueArray *arr, Value value);
void printValue(Value value);
void freeValueArray(ValueArray *arr);

#endif
