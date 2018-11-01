#ifndef MEDIA_TYPES_TEST_H
#define MEDIA_TYPES_TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "testLib.h"
#include "mediaTypes.h"


int mainTester();

void testAudioBelongsToTypes();
void testTrashDoesNotBelongToTypes();
void testCapitalizedTypeBelongsToTypes();
void testSplitOfDelimeterString();
void testSplitOfNull();
void testSplitOfSplittingString();
void testSplitOfNonSplittingString();
void testNullBelongsToText();
void testTextPlainBelongsToText();
void testTextTrashBelongsToTextPlain();
void testStressUserInput();

void givenNullMediaType();
void givenTextPlainMediaRange();
void givenTextTrashMediaType();
void givenTextMediaRange();
void givenTextPlainMediaType();
void givenStringAudio();
void givenStringTrash();
void givenCapitalizedType();
void givenADelimeterString();
void givenASplittingString();
void givenANonSplittingString();
void givenNullString();
void givenALargeUserInput();

void whenCheckingIfMediaTypeIsInMediaRange();
void whenCheckingContainment();
void whenSplittingString();
void whenSplittingUserInput();

void thenStringOneIsEqualToTest();
void thenStringTwoIsEqualToSplit();
void thenStringThreeIsEqualToNull();
void thenReturnedValueIsOne();
void thenReturnedValueIsZero();
void thenStringOneIsEqualToNoSplit();
void thenStringTwoIsEqualToNull();
void thenStringOneIsEqualToNull();
void thenStringThreeIsEqualToStress();
void thenStringFourIsEqualToUser();
void thenStringFiveIsEqualToInput();
void thenStringSixIsEqualToUnit();

void freeSplitStrings();

#endif
