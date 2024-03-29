/*------------------------------------------------------
   CCmdLine

   A utility for parsing command lines.

   Copyright (C) 1999 Chris Losinger, Smaller Animals Software.
   http://www.smalleranimals.com

   This software is provided 'as-is', without any express
   or implied warranty.  In no event will the authors be 
   held liable for any damages arising from the use of this software.

   Permission is granted to anyone to use this software 
   for any purpose, including commercial applications, and 
   to alter it and redistribute it freely, subject to the 
   following restrictions:

     1. The origin of this software must not be misrepresented; 
   you must not claim that you wrote the original software. 
   If you use this software in a product, an acknowledgment 
   in the product documentation would be appreciated but is not required.
   
     2. Altered source versions must be plainly marked as such, 
   and must not be misrepresented as being the original software.
   
     3. This notice may not be removed or altered from any source 
   distribution.

  -------------------------

   Example :

   Our example application uses a command line that has two
   required switches and two optional switches. The app should abort
   if the required switches are not present and continue with default
   values if the optional switches are not present.

      Sample command line : 
      MyApp.exe -p1 text1 text2 -p2 "this is a big argument" -opt1 -55 -opt2

      Switches -p1 and -p2 are required. 
      p1 has two arguments and p2 has one.
      
      Switches -opt1 and -opt2 are optional. 
      opt1 requires a numeric argument. 
      opt2 has no arguments.
      
      Also, assume that the app displays a 'help' screen if the '-h' switch
      is present on the command line.

   #include "CmdLine.h"

   void _tmain(int argc, TCHAR **argv)
   {
      // our cmd line parser object
      CCmdLine cmdLine;

      // parse argc,argv 
      if (cmdLine.SplitLine(argc, argv) < 1)
      {
         // no switches were given on the command line, abort
         ASSERT(0);
         exit(-1);
      }

      // test for the 'help' case
      if (cmdLine.HasSwitch(_T("-h")))
      {
         show_help();
         exit(0);
      }

      // get the required arguments
      StringType p1_1, p1_2, p2_1;
      try
      {  
         // if any of these fail, we'll end up in the catch() block
         p1_1 = cmdLine.GetArgument(_T("-p1"), 0);
         p1_2 = cmdLine.GetArgument(_T("-p1"), 1);
         p2_1 = cmdLine.GetArgument(_T("-p2"), 0);

      }
      catch (...)
      {
         // one of the required arguments was missing, abort
         ASSERT(0);
         exit(-1);
      }

      // get the optional parameters

      // convert to an int, default to '100'
      int iOpt1Val = atoi(cmdLine.GetSafeArgument(_T("-opt1"), 0, 100));

      // since opt2 has no arguments, just test for the presence of
      // the '-opt2' switch
      bool bOptVal2 = cmdLine.HasSwitch(_T("-opt2"));

      .... and so on....

   }

   If this class is used in an MFC application, StringType is CString, else
   it uses the STL 'string' type.

   If this is an MFC app, you can use the __argc and __argv macros from
   you CYourWinApp::InitInstance() function in place of the standard argc 
   and argv variables. 

------------------------------------------------------*/
#ifndef _CMDLINE_H_
#define _CMDLINE_H_

#ifdef __AFX_H__
// if we're using MFC, use CStrings 
#define StringType CString
#else
// if we're not using MFC, use STL strings
#define StringType std::basic_string<TCHAR>
#endif

// tell the compiler to shut up
#pragma warning(disable: 4786)

// #include <iostream> // you may need this
#include <map>
#include <string>
#include <vector>
#include <tchar.h>

// handy little container for our argument vector
struct CCmdParam
{
   std::vector<StringType> m_strings;
};

// this class is actually a map of strings to vectors
typedef std::map<StringType, CCmdParam> _CCmdLine;

// the command line parser class
class CCmdLine : public _CCmdLine
{

public:
   /*------------------------------------------------------
      int CCmdLine::SplitLine(int argc, TCHAR **argv)

      parse the command line into switches and arguments.

      returns number of switches found
   ------------------------------------------------------*/
   size_t      SplitLine(int argc, TCHAR **argv);

   /*------------------------------------------------------
      bool CCmdLine::HasSwitch(const TCHAR *pSwitch)

      was the switch found on the command line ?

      ex. if the command line is : app.exe -a p1 p2 p3 -b p4 -c -d p5

      call                              return
      ----                              ------
      cmdLine.HasSwitch(_T("-a"))       true
      cmdLine.HasSwitch(_T("-z"))       false
   ------------------------------------------------------*/   
   bool        HasSwitch(const TCHAR *pSwitch);

   /*------------------------------------------------------

      StringType CCmdLine::GetSafeArgument(const TCHAR *pSwitch, int iIdx, const TCHAR *pDefault)

      fetch an argument associated with a switch . if the parameter at
      index iIdx is not found, this will return the default that you
      provide.

      example :
  
      command line is : app.exe -a p1 p2 p3 -b p4 -c -d p5

      call                                              return
      ----                                              ------
      cmdLine.GetSafeArgument(_T("-a"), 0, _T("zz"))    p1
      cmdLine.GetSafeArgument(_T("-a"), 1, _T("zz"))    p2
      cmdLine.GetSafeArgument(_T("-b"), 0, _T("zz"))    p4
      cmdLine.GetSafeArgument(_T("-b"), 1, _T("zz"))    zz

   ------------------------------------------------------*/

   StringType  GetSafeArgument(const TCHAR *pSwitch, size_t iIdx, const TCHAR *pDefault);

   /*------------------------------------------------------

      StringType CCmdLine::GetArgument(const TCHAR *pSwitch, int iIdx)

      fetch a argument associated with a switch. throws an exception 
      of (int)0, if the parameter at index iIdx is not found.

      example :
  
      command line is : app.exe -a p1 p2 p3 -b p4 -c -d p5

      call                                 return
      ----                                 ------
      cmdLine.GetArgument(_T("-a"), 0)     p1
      cmdLine.GetArgument(_T("-b"), 1)     throws (int)0, returns an empty string

   ------------------------------------------------------*/
   StringType  GetArgument(const TCHAR *pSwitch, size_t iIdx);

   /*------------------------------------------------------
      int CCmdLine::GetArgumentCount(const TCHAR *pSwitch)

      returns the number of arguments found for a given switch.

      returns -1 if the switch was not found

   ------------------------------------------------------*/
   size_t     GetArgumentCount(const TCHAR *pSwitch);

protected:
   /*------------------------------------------------------

   protected member function
   test a parameter to see if it's a switch :

   switches are of the form : -x
   where 'x' is one or more characters.
   the first character of a switch must be non-numeric!

   ------------------------------------------------------*/
   bool       IsSwitch(const TCHAR *pParam);
};

#endif // _CMDLINE_H_

