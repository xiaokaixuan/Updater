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

   See SACmds.h for more info.
------------------------------------------------------*/

// if you're using MFC, you'll need to un-comment this line
#include "StdAfx.h"

#include "CmdLine.h"
#include "crtdbg.h"

/*------------------------------------------------------
  int CCmdLine::SplitLine(int argc, TCHAR **argv)

  parse the command line into switches and arguments

  returns number of switches found
------------------------------------------------------*/
size_t CCmdLine::SplitLine(int argc, TCHAR **argv)
{
   clear();

   StringType curParam; // current argv[x]
   // skip the exe name (start with i = 1)
   for (int i = 1; i < argc; i++)
   {
      // if it's a switch, start a new CCmdLine
      if (IsSwitch(argv[i]))
      {
         curParam = argv[i];

         StringType arg;
         // look at next input string to see if it's a switch or an argument
         if (i + 1 < argc)
         {
            if (!IsSwitch(argv[i + 1]))
            {
               // it's an argument, not a switch
               arg = argv[i + 1];
               // skip to next
               i++;
            }
            else
            {
               arg = _T("");
            }
         }
         // add it
         CCmdParam cmd;
         // only add non-empty args
         if (arg != _T(""))
         {
            cmd.m_strings.push_back(arg);
         }
         // add the CCmdParam to 'this'
         std::pair<CCmdLine::iterator, bool> res = insert(CCmdLine::value_type(curParam, cmd));
      }
      else
      {
         // it's not a new switch, so it must be more stuff for the last switch

         // ...let's add it
 	      CCmdLine::iterator theIterator;

         // get an iterator for the current param
         theIterator = find(curParam);
	     if (theIterator != end())
         {
            (*theIterator).second.m_strings.push_back(argv[i]);
         }
         else
         {
            // ??
         }
      }
   }

   return size();
}

/*------------------------------------------------------

   protected member function
   test a parameter to see if it's a switch :

   switches are of the form : -x
   where 'x' is one or more TCHARacters.
   the first TCHARacter of a switch must be non-numeric!

------------------------------------------------------*/

bool CCmdLine::IsSwitch(const TCHAR *pParam)
{
   if (pParam == NULL)
      return false;

   // switches must non-empty
   // must have at least one TCHARacter after the '-'
   size_t len = _tcslen(pParam);
   if (len <= 1)
   {
      return false;
   }

   // switches always start with '-'
   if (pParam[0] == _T('-'))
   {
      // allow negative numbers as arguments.
      // ie., don't count them as switches
      return (!_istdigit(pParam[1]));
   }
   else
   {
      return false;
   }
}

/*------------------------------------------------------
   bool CCmdLine::HasSwitch(const TCHAR *pSwitch)

   was the switch found on the command line ?

   ex. if the command line is : app.exe -a p1 p2 p3 -b p4 -c -d p5

   call                             return
   ----                             ------
   cmdLine.HasSwitch(_T("-a"))       true
   cmdLine.HasSwitch(_T("-z"))       false
------------------------------------------------------*/

bool CCmdLine::HasSwitch(const TCHAR *pSwitch)
{
	CCmdLine::iterator theIterator;
	theIterator = find(pSwitch);
	return (theIterator != end());
}

/*------------------------------------------------------

   StringType CCmdLine::GetSafeArgument(const TCHAR *pSwitch, int iIdx, const TCHAR *pDefault)

   fetch an argument associated with a switch . if the parameter at
   index iIdx is not found, this will return the default that you
   provide.

   example :
  
   command line is : app.exe -a p1 p2 p3 -b p4 -c -d p5

   call                                            return
   ----                                            ------
   cmdLine.GetSafeArgument(_T("-a"), 0, _T("zz"))    p1
   cmdLine.GetSafeArgument(_T("-a"), 1, _T("zz"))    p2
   cmdLine.GetSafeArgument(_T("-b"), 0, _T("zz"))    p4
   cmdLine.GetSafeArgument(_T("-b"), 1, _T("zz"))    zz

------------------------------------------------------*/

StringType CCmdLine::GetSafeArgument(const TCHAR *pSwitch, size_t iIdx, const TCHAR *pDefault)
{
   StringType sRet;
   
   if (pDefault != NULL)
      sRet = pDefault;

   try
   {
      sRet = GetArgument(pSwitch, iIdx);
   }
   catch (...)
   {
   }

   return sRet;
}

/*------------------------------------------------------

   StringType CCmdLine::GetArgument(const TCHAR *pSwitch, int iIdx)

   fetch a argument associated with a switch. throws an exception 
   of (int)0, if the parameter at index iIdx is not found.

   example :
  
   command line is : app.exe -a p1 p2 p3 -b p4 -c -d p5

   call                               return
   ----                               ------
   cmdLine.GetArgument(_T("-a"), 0)     p1
   cmdLine.GetArgument(_T("-b"), 1)     throws (int)0, returns an empty string

------------------------------------------------------*/

StringType CCmdLine::GetArgument(const TCHAR *pSwitch, size_t iIdx)
{
   if (HasSwitch(pSwitch))
   {
	   CCmdLine::iterator theIterator;

      theIterator = find(pSwitch);
	   if (theIterator != end())
      {
         if ((*theIterator).second.m_strings.size() > iIdx)
         {
            return (*theIterator).second.m_strings[iIdx];
         }
      }
   }

   throw (int)0;
   return StringType(_T(""));
}

/*------------------------------------------------------
   int CCmdLine::GetArgumentCount(const TCHAR *pSwitch)

   returns the number of arguments found for a given switch.

   returns -1 if the switch was not found

------------------------------------------------------*/

size_t CCmdLine::GetArgumentCount(const TCHAR *pSwitch)
{
   size_t iArgumentCount(0);

   if (HasSwitch(pSwitch))
   {
	   CCmdLine::iterator theIterator;

      theIterator = find(pSwitch);
	   if (theIterator != end())
      {
         iArgumentCount = (*theIterator).second.m_strings.size();
      }
   }

   return iArgumentCount;
}
