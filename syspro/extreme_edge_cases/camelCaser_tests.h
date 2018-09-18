/**
* Extreme Edge Cases Lab
* CS 241 - Fall 2018
*/

#pragma once

#include "camelCaser.h"

int test_camelCaser(char **(*camelCaser)(const char *),
                    void (*destroy)(char **));
