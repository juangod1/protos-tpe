#include "mediaTypesTest.h"


char * testMediaType[2];
char * testMediaRange[2];
char ** testSplitStrings;
char * testString;
int testReturnValue;

int mainTester()
{
  testAudioBelongsToTypes();
  testTrashDoesNotBelongToTypes();
  testCapitalizedTypeBelongsToTypes();
  testSplitOfNull();
  testSplitOfSplittingString();
  testSplitOfNonSplittingString();
  testSplitOfDelimeterString();
  testTextPlainBelongsToText();
  testTextTrashBelongsToTextPlain();
  testNullBelongsToText();
  testStressUserInput();
  return 0;
}

void testStressUserInput()
{
  printf("Entering user input stress test\n");
  givenALargeUserInput();
  whenSplittingUserInput();
  thenStringOneIsEqualToTest();
  thenStringTwoIsEqualToSplit();
  thenStringThreeIsEqualToStress();
  thenStringFourIsEqualToUser();
  thenStringFiveIsEqualToInput();
  thenStringSixIsEqualToUnit();
  freeSplitStrings();
}

void givenALargeUserInput()
{
  testString="test\nsplit\nstress\nuser\ninput\nunit\n";
}
void whenSplittingUserInput()
{
  testSplitStrings = divideUserInputByLine(testString);
}

void thenStringThreeIsEqualToStress()
{
  if(strcmp(testSplitStrings[2],"stress")==0)
  {
    ok();
  }
  else
  {
    fail("Expected string equal to stress, found different string");
  }
}

void thenStringFourIsEqualToUser()
{
  if(strcmp(testSplitStrings[3],"user")==0)
  {
    ok();
  }
  else
  {
    fail("Expected string equal to user, found different string");
  }
}

void thenStringFiveIsEqualToInput()
{
  if(strcmp(testSplitStrings[4],"input")==0)
  {
    ok();
  }
  else
  {
    fail("Expected string equal to input, found different string");
  }
}

void thenStringSixIsEqualToUnit()
{
  if(strcmp(testSplitStrings[5],"unit")==0)
  {
    ok();
  }
  else
  {
    fail("Expected string equal to unit, found different string");
  }
}

void testNullBelongsToText()
{
  printf("Entering null belongs to text test\n");
  givenNullMediaType();
  givenTextMediaRange();
  whenCheckingIfMediaTypeIsInMediaRange();
  thenReturnedValueIsZero();
}

void givenNullMediaType()
{
  testMediaType[0]=NULL;
  testMediaType[1]=NULL;
}

void testTextTrashBelongsToTextPlain()
{
  printf("Entering text trash belongs to text plain test\n");
  givenTextTrashMediaType();
  givenTextPlainMediaRange();
  whenCheckingIfMediaTypeIsInMediaRange();
  thenReturnedValueIsZero();
}

void givenTextPlainMediaRange()
{
  testMediaRange[0]="text";
  testMediaRange[1]="plain";
}

void givenTextTrashMediaType()
{
  testMediaType[0]="text";
  testMediaType[1]="trash";
}

void testTextPlainBelongsToText()
{
  printf("Entering text plain belongs to text test\n");
  givenTextPlainMediaType();
  givenTextMediaRange();
  whenCheckingIfMediaTypeIsInMediaRange();
  thenReturnedValueIsOne();
}

void givenTextMediaRange()
{

  testMediaRange[1]="text";
  testMediaRange[1]="*";
}

void givenTextPlainMediaType()
{
  testMediaType[1]="text";
  testMediaType[1]="plain";
}

void whenCheckingIfMediaTypeIsInMediaRange()
{
  testReturnValue = mediaTypeBelongsToMediaRange(testMediaType,testMediaRange);
}

void testSplitOfDelimeterString()
{
  printf("Entering split of delimeter string test\n");
  givenADelimeterString();
  whenSplittingString();
  thenStringOneIsEqualToNull();
  freeSplitStrings();
}

void testSplitOfNonSplittingString()
{
  printf("Entering split of non splitting string test\n");
  givenANonSplittingString();
  whenSplittingString();
  thenStringOneIsEqualToNoSplit();
  thenStringTwoIsEqualToNull();
  freeSplitStrings();
}

void testSplitOfSplittingString()
{
  printf("Entering split of splitting string test\n");
  givenASplittingString();
  whenSplittingString();
  thenStringOneIsEqualToTest();
  thenStringTwoIsEqualToSplit();
  thenStringThreeIsEqualToNull();
  freeSplitStrings();
}

void givenADelimeterString()
{
  testString = "/////";
}

void givenANonSplittingString()
{
  testString = "nosplit";
}

void givenASplittingString()
{
  testString="test/split";
}

void thenStringOneIsEqualToTest()
{
  if(strcmp(testSplitStrings[0],"test")==0)
  {
    ok();
  }
  else
  {
    fail("Expected string equal to test, found different string");
  }
}

void thenStringOneIsEqualToNoSplit()
{
  if(strcmp(testSplitStrings[0],"nosplit")==0)
  {
    ok();
  }
  else
  {
    fail("Expected string equal to nosplit, found different string");
  }
}

void thenStringTwoIsEqualToSplit()
{
  if(strcmp(testSplitStrings[1],"split")==0)
  {
    ok();
  }
  else
  {
    fail("Expected string equal to split, found different string");
  }
}

void thenStringTwoIsEqualToNull()
{
  if(testSplitStrings[1]==NULL)
  {
    ok();
  }
  else
  {
    fail("Expected NULL string, found different string");
  }
}

void thenStringThreeIsEqualToNull()
{
  if(testSplitStrings[2]==NULL)
  {
    ok();
  }
  else
  {
    fail("Expected NULL string, found different string");
  }
}

void testSplitOfNull()
{
  printf("Entering split of null test\n");
  givenNullString();
  whenSplittingString();
  thenStringOneIsEqualToNull();
  freeSplitStrings();
}

void givenNullString()
{
  testString=NULL;
}

void whenSplittingString()
{
  testSplitStrings = divideMediaType(testString);
}

void thenStringOneIsEqualToNull()
{
  if(testSplitStrings[0]==NULL)
  {
    ok();
  }
  else
  {
    fail("Expected return value NULL, found different value");
  }
}

void freeSplitStrings()
{
  if(testSplitStrings==NULL){
    return;
  }
  int i=0;
  while((testSplitStrings[i])!=NULL)
  {
    free(testSplitStrings[i]);
    testSplitStrings[i]=NULL;
    i++;
  }
  free(testSplitStrings);
  testSplitStrings=NULL;
}


void testAudioBelongsToTypes()
{
  printf("Entering 'audio' belongs to types test\n");
  givenStringAudio();
  whenCheckingContainment();
  thenReturnedValueIsOne();
}

void testTrashDoesNotBelongToTypes()
{
  printf("Entering 'trash' does not belong to types test\n");
  givenStringTrash();
  whenCheckingContainment();
  thenReturnedValueIsZero();
}

void testCapitalizedTypeBelongsToTypes()
{
  printf("Entering 'AUDIO' belongs to types test\n");
  givenCapitalizedType();
  whenCheckingContainment();
  thenReturnedValueIsOne();
}

void givenStringTrash()
{
  testString="trash";
}

void givenStringAudio()
{
  testString="audio";
}

void givenCapitalizedType()
{
  testString="AUDIO";
}

void whenCheckingContainment()
{
  testReturnValue = isType(testString);
}

void thenReturnedValueIsZero()
{
  if(testReturnValue==0)
  {
    ok();
  }
  else
  {
    fail("Expected return value 0, found different value");
  }
}

void thenReturnedValueIsOne()
{
  if(testReturnValue==1)
  {
    ok();
  }
  else
  {
    fail("Expected return value 1, found different value");
  }
}
