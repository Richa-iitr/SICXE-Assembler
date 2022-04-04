// PASS1 : Defines symbols and literals and create symbol tab and literal table
#include "Functions.cpp"
#include "Tables.cpp"

using namespace std;

string fileName;      //name of the input file as entered by the user
bool error_flag = false;
int program_length;
string *BLocksNumToName;

string firstExecutable_Sec;

// functions for pass1

void forLTORG(string &literalPre, int &_lineNumber, int lineNumber, int &LOCCTR, int &lastDeltaLOCCTR, int currentBlockNumber);
void evaluateExpression(string expression, bool &relative, string &tempOperand, int lineNumber, ofstream &errorFile, bool &error_flag);
void pass1();

//--------------------------------------------------LOGIC FOR PASS1--------------------------------------------------------
/* In the pass1 stage of the assembler we generate the intermediate file which contains line number, address, opcode, labels, 
*/
void pass1()
{
  ifstream inputFile; // begin
  ofstream intermediate_File, errorFile;

  inputFile.open(fileName);
  if (!inputFile)
  {
    cout << "Unable to open file: " << fileName << endl;
    exit(1);
  }

  intermediate_File.open("intermediatePass1_" + fileName);
  if (!intermediate_File)
  {
    cout << "Unable to open file: intermediatePass1_" << fileName<< endl;
    exit(1);
  }
  writeToFile(intermediate_File, "Line\tAddress\tBlockNumber\tLabel\tOPCODE\tOPERAND\tComment");
  errorFile.open("error_" + fileName);
  if (!errorFile)
  {
    cout << "Unable to open file: error" << fileName << endl;
    exit(1);
  }
  writeToFile(errorFile, "*****************************");
  writeToFile(errorFile, "            PASS1            ");
  writeToFile(errorFile, "*****************************");

  string fileLine;
  string writeData, writeSuffixData = "", writeDataPrefix = "";
  int index = 0;

  string currentBlockName = "DEFAULT";
  int currentBlockNumber = 0;
  int totalBlocks = 1;

  bool statusCode;
  string label, opcode, operand, comment;
  string tempOperand;

  int startAddress, LOCCTR, saveLOCCTR, lineNumber, lastLOCCTR, _lineNumber = 0;
  lineNumber = 0;
  lastLOCCTR = 0;

  getline(inputFile, fileLine);
  lineNumber += 5;
  while (checkCommentLine(fileLine))
  {
    // write comments to the file
    writeData = to_string(lineNumber) + "\t" + fileLine;
    writeToFile(intermediate_File, writeData);
    getline(inputFile, fileLine); 
    lineNumber += 5;
    index = 0;
  }

  firstNonWhiteSpace(fileLine, index, statusCode, label);
  firstNonWhiteSpace(fileLine, index, statusCode, opcode);

  //PARSING THE INPUT PROGRAM
  if (opcode == "START")
  { //start with START 
    firstNonWhiteSpace(fileLine, index, statusCode, operand); // Parse label
    firstNonWhiteSpace(fileLine, index, statusCode, comment, true);
    startAddress = stringHexToInt(operand);
    LOCCTR = startAddress;
    writeData = to_string(lineNumber) + "\t" + intToStringHex(LOCCTR - lastLOCCTR) + "\t" + to_string(currentBlockNumber) + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + comment;
    writeToFile(intermediate_File, writeData); // Write file to intermediate file

    getline(inputFile, fileLine); // Read next line
    lineNumber += 5;
    index = 0;
    firstNonWhiteSpace(fileLine, index, statusCode, label);  
    firstNonWhiteSpace(fileLine, index, statusCode, opcode);
  }
  else
  {
    startAddress = 0;
    LOCCTR = 0;
  }
  string currentSection = "DEFAULT";  /////////////////////////////////////////
  int SECCTR = 0;                     /////////////////////////////////////////

  //Loop till the end of the input code
  while (opcode != "END")
  {
    //if not comment
      if (!checkCommentLine(fileLine))
      { //if label exists
        if (label != "")
        { //if the label doesn't exist in symbol table, add it
          if (SYMTAB[label].exists == 'n')
          {
            SYMTAB[label].name = label;
            SYMTAB[label].address = intToStringHex(LOCCTR);
            SYMTAB[label].relative = 1;
            SYMTAB[label].exists = 'y';
            SYMTAB[label].blockNumber = currentBlockNumber;
          }
          else
          {
            writeData = "Line: " + to_string(lineNumber) + " : Symbol redefined: '" + label + "'. Already defined at " + SYMTAB[label].address + " Code: 06";
            writeToFile(errorFile, writeData);
            error_flag = true;
          }
        }
        if (OPTAB[getOpcode(opcode)].exists == 'y')
        { // Search for opcode in OPTAB
          // **********************************************format-1**********************************************************
          if (OPTAB[getOpcode(opcode)].format == 1)
          {
            operand = " ";
            LOCCTR += OPTAB[getOpcode(opcode)].format;
            lastLOCCTR += OPTAB[getOpcode(opcode)].format;
          }
          // **********************************************format-3**********************************************************
          else if (OPTAB[getOpcode(opcode)].format == 3)
          {
            LOCCTR += 3;
            lastLOCCTR += 3;
            //*********************************************format 4**********************************************************
            if (getOperandFormat(opcode) == '+')
            {
              LOCCTR += 1;
              lastLOCCTR += 1;
            }
            if (getOpcode(opcode) == "RSUB")
            {
              operand = " ";
            }
            else
            {
              firstNonWhiteSpace(fileLine, index, statusCode, operand);
              if (operand[operand.length() - 1] == ',')
              {
                firstNonWhiteSpace(fileLine, index, statusCode, tempOperand);
                operand += tempOperand;
              }
            }
            //Literals
            if (getOperandFormat(operand) == '=')
            {
              tempOperand = operand.substr(1, operand.length() - 1);
              if (tempOperand == "*")
              {
                tempOperand = "X'" + intToStringHex(LOCCTR - lastLOCCTR, 6) + "'";
              }
              if (LITTAB[tempOperand].exists == 'n')
              {
                LITTAB[tempOperand].value = tempOperand;
                LITTAB[tempOperand].exists = 'y';
                LITTAB[tempOperand].address = "?";
                LITTAB[tempOperand].blockNumber = -1;
              }
            }
          }
          
          // **********************************************format-2**********************************************************
          else
          {
            LOCCTR += OPTAB[getOpcode(opcode)].format;
            lastLOCCTR += OPTAB[getOpcode(opcode)].format;
            firstNonWhiteSpace(fileLine, index, statusCode, operand);
            if (operand[operand.length() - 1] == ',')
            {
              firstNonWhiteSpace(fileLine, index, statusCode, tempOperand);
              operand += tempOperand;
            }
          }
        }
        //Reserves words
        else if (opcode == "RESW")
        {
          firstNonWhiteSpace(fileLine, index, statusCode, operand);
          LOCCTR += 3 * stoi(operand);
          lastLOCCTR += 3 * stoi(operand);
        }
        //Reserve Bytess
        else if (opcode == "RESB")
        {
          firstNonWhiteSpace(fileLine, index, statusCode, operand);
          LOCCTR += stoi(operand);
          lastLOCCTR += stoi(operand);
        }
        //Initializes a word
        else if (opcode == "WORD")
        {
          firstNonWhiteSpace(fileLine, index, statusCode, operand);
          LOCCTR += 3;
          lastLOCCTR += 3;
        }
        else if (opcode == "BYTE")
        {
          readByteOperand(fileLine, index, statusCode, operand);
          if (operand[0] == 'C')
          {
            LOCCTR += operand.length() - 3;
            lastLOCCTR += operand.length() - 3;
          } 
          else if (operand[0] == 'X')
          {
            LOCCTR += (operand.length() - 3) / 2;
            lastLOCCTR += (operand.length() - 3) / 2;
          } 
          else {
            writeData ="Line: " + to_string(lineNumber) + " Invalid BYTE Operand!!. Provided type: " + to_string(operand[0]) + ". Code: 07 " ;
          }
        }
        //BYTE: sets the base register
        else if (opcode == "BASE")
        {
          firstNonWhiteSpace(fileLine, index, statusCode, operand);
        }
        //LTORG: to set position 
        else if (opcode == "LTORG")
        {
          operand = " ";
          forLTORG(writeSuffixData, _lineNumber, lineNumber, LOCCTR, lastLOCCTR, currentBlockNumber);
        }
        //ORG 
        else if (opcode == "ORG")
        {
          firstNonWhiteSpace(fileLine, index, statusCode, operand);

          char lastByte = operand[operand.length() - 1];
          while (lastByte == '+' || lastByte == '-' || lastByte == '/' || lastByte == '*')
          {
            firstNonWhiteSpace(fileLine, index, statusCode, tempOperand);
            operand += tempOperand;
            lastByte = operand[operand.length() - 1];
          }
          //setting the Location counter
          int tempVariable;
          tempVariable = saveLOCCTR;
          saveLOCCTR = LOCCTR;
          LOCCTR = tempVariable;

          if (SYMTAB[operand].exists == 'y')
          {
            LOCCTR = stringHexToInt(SYMTAB[operand].address);
          }
          else
          {
            bool relative;
            error_flag = false;
            evaluateExpression(operand, relative, tempOperand, lineNumber, errorFile, error_flag);
            if (!error_flag)
            {
              LOCCTR = stringHexToInt(tempOperand);
            }
            error_flag = false;
          }
        }
        //USE: program blocks
        else if (opcode == "USE")
        {

          firstNonWhiteSpace(fileLine, index, statusCode, operand);
          BLOCKS[currentBlockName].LOCCTR = intToStringHex(LOCCTR);

          if (BLOCKS[operand].exists == 'n')
          {
            BLOCKS[operand].exists = 'y';
            BLOCKS[operand].name = operand;
            BLOCKS[operand].number = totalBlocks++;
            BLOCKS[operand].LOCCTR = "0";
          }
          currentBlockNumber = BLOCKS[operand].number;
          currentBlockName = BLOCKS[operand].name;
          LOCCTR = stringHexToInt(BLOCKS[operand].LOCCTR);
        }
        //EQU: evaluating expression
        else if (opcode == "EQU")
        {
          firstNonWhiteSpace(fileLine, index, statusCode, operand);
          tempOperand = "";
          bool relative;
          //current address
          if (operand == "*")
          {
            tempOperand = intToStringHex(LOCCTR - lastLOCCTR, 6);
            relative = 1;
          }
          else if (ifAllNum(operand))
          {
            tempOperand = intToStringHex(stoi(operand), 6);
            relative = 0;
          }
          else
          {
            char lastByte = operand[operand.length() - 1];

            while (lastByte == '+' || lastByte == '-' || lastByte == '/' || lastByte == '*')
            {
              firstNonWhiteSpace(fileLine, index, statusCode, tempOperand);
              operand += tempOperand;
              lastByte = operand[operand.length() - 1];
            }

            // Code for reading whole operand
            evaluateExpression(operand, relative, tempOperand, lineNumber, errorFile, error_flag);
          }

          SYMTAB[label].name = label;
          SYMTAB[label].address = tempOperand;
          SYMTAB[label].relative = relative;
          SYMTAB[label].blockNumber = currentBlockNumber;
          lastLOCCTR = LOCCTR - stringHexToInt(tempOperand);
        }
        else
        {
          firstNonWhiteSpace(fileLine, index, statusCode, operand);
          writeData = "Line: " + to_string(lineNumber) + " : Invalid OPCODE. Found " + opcode;
          writeToFile(errorFile, writeData);
          error_flag = true;
        }
        firstNonWhiteSpace(fileLine, index, statusCode, comment, true);
        if (opcode == "EQU" && SYMTAB[label].relative == 0)
        {
          writeData = writeDataPrefix + to_string(lineNumber) + "\t" + intToStringHex(LOCCTR - lastLOCCTR) + "\t" + " " + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + comment + writeSuffixData;
        }
        else
        {
          writeData = writeDataPrefix + to_string(lineNumber) + "\t" + intToStringHex(LOCCTR - lastLOCCTR) + "\t" + to_string(currentBlockNumber) + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + comment + writeSuffixData;
        }
        writeDataPrefix = "";
        writeSuffixData = "";
      }
      else
      {
        writeData = to_string(lineNumber) + "\t" + fileLine;
      }
      writeToFile(intermediate_File, writeData);

      BLOCKS[currentBlockName].LOCCTR = intToStringHex(LOCCTR); // Update LOCCTR of block after every instruction
      getline(inputFile, fileLine);                            // Read next line
      lineNumber += 5 + _lineNumber;
      _lineNumber = 0;
      index = 0;
      lastLOCCTR = 0;
      firstNonWhiteSpace(fileLine, index, statusCode, label);  // Parse label
      firstNonWhiteSpace(fileLine, index, statusCode, opcode); // Parse OPCODE
    }
    //*****************************************************************************************

    if (opcode != "END")
    {

      if (SYMTAB[label].exists == 'n')
      {
        SYMTAB[label].name = label;
        SYMTAB[label].address = intToStringHex(LOCCTR);
        SYMTAB[label].relative = 1;
        SYMTAB[label].exists = 'y';
        SYMTAB[label].blockNumber = currentBlockNumber;
      }

      writeToFile(intermediate_File, writeDataPrefix + to_string(lineNumber) + "\t" + intToStringHex(LOCCTR - lastLOCCTR) + "\t" + to_string(currentBlockNumber) + "\t" + label + "\t" + opcode);

      getline(inputFile, fileLine); // Read next line
      lineNumber += 5;

      firstNonWhiteSpace(fileLine, index, statusCode, label);  // Parse label
      firstNonWhiteSpace(fileLine, index, statusCode, opcode); // Parse OPCODE
  }

  if (opcode == "END")
  {
    firstExecutable_Sec = SYMTAB[label].address;
    SYMTAB[firstExecutable_Sec].name = label;
    SYMTAB[firstExecutable_Sec].address = firstExecutable_Sec;
  }

  firstNonWhiteSpace(fileLine, index, statusCode, operand);
  firstNonWhiteSpace(fileLine, index, statusCode, comment, true);

  /*Change to deafult before dumping literals*/
  currentBlockName = "DEFAULT";
  currentBlockNumber = 0;
  LOCCTR = stringHexToInt(BLOCKS[currentBlockName].LOCCTR);

  forLTORG(writeSuffixData, _lineNumber, lineNumber, LOCCTR, lastLOCCTR, currentBlockNumber);

  writeData = to_string(lineNumber) + "\t" + intToStringHex(LOCCTR - lastLOCCTR) + "\t" + " " + "\t" + label + "\t" + opcode + "\t" + operand + "\t" + comment + writeSuffixData;
  writeToFile(intermediate_File, writeData);

  int LocctrArr[totalBlocks];
  BLocksNumToName = new string[totalBlocks];
  for (auto const &it : BLOCKS)
  {
    LocctrArr[it.second.number] = stringHexToInt(it.second.LOCCTR);
    BLocksNumToName[it.second.number] = it.first;
  }

  for (int i = 1; i < totalBlocks; i++)
  {
    LocctrArr[i] += LocctrArr[i - 1];
  }

  for (auto const &it : BLOCKS)
  {
    if (it.second.startAddress == "?")
    {
      BLOCKS[it.first].startAddress = intToStringHex(LocctrArr[it.second.number - 1]);
    }
  }

  program_length = LocctrArr[totalBlocks - 1] - startAddress;

  inputFile.close();
  intermediate_File.close();
  errorFile.close();
}

//----------------------------------------evaluating the expression (e.g. BUFFER+BUFEND-1)------------------------------------------------------------------
/*expression: the expression to be parsed or evaluated
*relative: boolean which checks whether it is relative or not, passed as reference as we need the modified value later
*tempOperand: temporarily stores the result of avaulated expression
*lineNumber: the line number we are on
*errorFile: this file stores all the errors which might have occured 
*error_flag: stores the boolean whether error occured or not
*/
void evaluateExpression(string expression, bool &relative, string &tempOperand, int lineNumber, ofstream &errorFile, bool &error_flag)
{
  //some local variables to evaluate the expression
  string singleOperand = "?", singleOperator = "?", valueString = "", valueTemp = "", writeData = "";
  int lastOperand = 0, lastOperator = 0, pairCount = 0;
  //pairCount: counts the pairs of relative terms to check if the expression is valid 
  char lastByte = ' ';
  bool Illegal = false;

  for (int i = 0; i < expression.length();)
  {
    singleOperand = "";
    lastByte = expression[i];
    
    // while we are getting valid entries not marking the end or stop point 
    while ((lastByte != '+' && lastByte != '-' && lastByte != '/' && lastByte != '*') && i < expression.length())
    {
      singleOperand += lastByte;
      lastByte = expression[++i];
    }

  // if the operand exists in symbol table. store the address in temp value of the symbol
    if (SYMTAB[singleOperand].exists == 'y')
    { 
      lastOperand = SYMTAB[singleOperand].relative;
      valueTemp = to_string(stringHexToInt(SYMTAB[singleOperand].address));
    }
    else if ((singleOperand != "" || singleOperand != "?") && ifAllNum(singleOperand))
    {
      lastOperand = 0;
      valueTemp = singleOperand;
    }
    else
    {
      //if the symbol doesn't exist in symbol table genrate error message in the file and stop checking further.
      writeData = "Line: " + to_string(lineNumber) + " : Symbol doesn't exist. Found " + singleOperand + ". Code 02.";
      writeToFile(errorFile, writeData);
      Illegal = true;
      break;
    }

    // Check if the expressions are valid
    if (lastOperand * lastOperator == 1)
    { 
      writeData = "Line: " + to_string(lineNumber) + " : Illegal expression!! Code 03.";
      writeToFile(errorFile, writeData);
      error_flag = true;
      Illegal = true;
      break;
    }
    else if ((singleOperator == "-" || singleOperator == "+" || singleOperator == "?") && lastOperand == 1)
    {
      if (singleOperator == "-")
      {
        pairCount--;
      }
      else
      {
        pairCount++;
      }
    }

    valueString += valueTemp;

    singleOperator = "";
    while (i < expression.length() && (lastByte == '+' || lastByte == '-' || lastByte == '/' || lastByte == '*'))
    {
      singleOperator += lastByte;
      lastByte = expression[++i];
    }

    //if the operator's length is more than 1 then generate error 
    if (singleOperator.length() > 1)
    {
      writeData = "Line: " + to_string(lineNumber) + " : Illegal operator found in expression. Found " + singleOperator + ". Code 04.";
      writeToFile(errorFile, writeData);
      error_flag = true;
      Illegal = true;
      break;
    }
  
    //multiplication and division 
    if (singleOperator == "*" || singleOperator == "/")
    {
      lastOperator = 1;
    }
    else
    {
      lastOperator = 0;
    }
    valueString += singleOperator;
  }

  //if the expression is valid
  if (!Illegal)
  {
    //relative
    if (pairCount == 1)
    {
      relative = 1;
      EvaluateString evstr(valueString);
      tempOperand = intToStringHex(evstr.getRes());
    }
    else if (pairCount == 0)    //absolute
    {
      relative = 0;
      cout << valueString << endl;
      EvaluateString evstr(valueString);
      tempOperand = intToStringHex(evstr.getRes());
    }
    else
    {
      //generating error message
      writeData = "Line: " + to_string(lineNumber) + " : Invalid expression!! Code: 05.";
      writeToFile(errorFile, writeData);
      error_flag = true;
      tempOperand = "00000";
      relative = 0;
    }
  }
  else
  {
    tempOperand = "00000";
    error_flag = true;
    relative = 0;
  }
}

// ----------------------------------------FUNCTION TO HANDLE LTORG ASSEMBLER DIRECTIVE----------------------------------------------
/*literalPre: the prefix of the literal in literal table include line number, location counter, block number, the literal value, passed as reference
 *_lineNumber: the change in line numbers 
 *lineNumber: the line number
 *LOCCTR: location counter
 *lastLOCCTR: the last location counter
 *currentBlockNumber: the current block number
*/
void forLTORG(string &literalPre, int &_lineNumber, int lineNumber, int &LOCCTR, int &lastLOCCTR, int currentBlockNumber)
{
  string litAddress, litValue;
  literalPre = "";
  for (auto const &it : LITTAB)
  {
    litAddress = it.second.address;
    litValue = it.second.value;
    if (litAddress != "?")
    {
      /*Pass as already address is assigned*/
    }
    else
    {
      lineNumber += 5;
      _lineNumber += 5;
      LITTAB[it.first].address = intToStringHex(LOCCTR);
      LITTAB[it.first].blockNumber = currentBlockNumber;
      literalPre += "\n" + to_string(lineNumber) + "\t" + intToStringHex(LOCCTR) + "\t" + to_string(currentBlockNumber) + "\t" + "*" + "\t" + "=" + litValue + "\t" + " " + "\t" + " ";

      if (litValue[0] == 'X')
      {
        LOCCTR += (litValue.length() - 3) / 2;
        lastLOCCTR += (litValue.length() - 3) / 2;
      }
      else if (litValue[0] == 'C')
      {
        LOCCTR += litValue.length() - 3;
        lastLOCCTR += litValue.length() - 3;
      }
    }
  }
}
