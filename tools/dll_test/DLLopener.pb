;'DllFunctionDump'. 
;----------------- 
;   Stephen Rodriguez. 
;   Created with Purebasic 4.02 for Windows. 
; 
;   Based on some code by Torsten Rienow;  http://www.powerbasic.com/support/forums/Forum7/HTML/001216.html 

;   Date:  March 2007. 
; 
;   Platforms:  Windows only.  Tested on XP professional SP2. 
; 
;Licence. 
;------- 
;   You are not allowed to use this code or any derivative for any hacking (api hijacking) 
;   tools. 
; 
;   No warranties... etc. 
;****************************************************************************************** 
; 
;The function GetDllFunctions() will retrieve all exported function names residing within 
;the specified dll/exe file.  The names are placed within a string array passed as a parameter 
;to the Procedure. 
;The function returns a count of the number of functions retrieved. 
;It will also return zero in the case of an error. 
; 
;The array will be redimensioned as appropriate, 
;****************************************************************************************** 


#IMAGE_DOS_SIGNATURE = $5A4D 
#IMAGE_NT_SIGNATURE = $4550 

;The following structure is not already defined by PB, 
Structure _IMAGE_SECTION_HEADER 
  Name.b[8]    ;8 bytes for a null-padded section name. UTF 8 format. 
  VirtualSize.l ;= 0 
  VirtualAddress.l ; = 0 
  SizeOfRawData.l 
  PointerToRawData.l 
  PointerToRelocations.l 
  PointerToLineNumbers.l ; = 0 
  NumberOfRelocations.w 
  NumberOfLineNumbers.w ; = 0 
  Characteristics.l 
EndStructure 



Procedure.l GetDllFunctions(filename.s,  Array strTable.s(1)) 
  Protected filesize, fileid, hFileMapping, filebase 
  Protected *idh.IMAGE_DOS_HEADER, *inth.IMAGE_NT_HEADERS 
  Protected *ExpDir.IMAGE_EXPORT_DIRECTORY, *section._IMAGE_SECTION_HEADER, *exsection._IMAGE_SECTION_HEADER 
  Protected exportsStartRVA, exportsEndRVA, entryPointRVA 
  Protected delta, *pFunctions.LONG, *pNames.LONG, currentsymname$ 
  Protected flag, i, j, count, dirsize 
  ;First job, check the filename. 
    filesize = FileSize(filename) 
    If filesize <=0 : ProcedureReturn 0 : EndIf 
    fileid=ReadFile(#PB_Any, filename) 
    If fileid = 0 : ProcedureReturn 0 : EndIf 
  ;Create a file mapping object so that we can treat the dll as if it were in memory. 
    hFileMapping = CreateFileMapping_(FileID(fileid), 0, #PAGE_READONLY, 0, 0, 0) 
    If hFileMapping = 0 : CloseFile(fileid) : ProcedureReturn 0 : EndIf 
  ;Map the file. 
    FileBase = MapViewOfFile_(hFileMapping, #FILE_MAP_READ, 0, 0, 0) 
    If FileBase  = 0 
      CloseHandle_(hFileMapping) 
      CloseFile(fileid) 
      ProcedureReturn 0 
    EndIf 
    *idh=FileBase 
    If *idh\e_magic = #IMAGE_DOS_SIGNATURE 
      *inth=*idh + *idh\e_lfanew 
      ;Check that the current process has access to the memory and the image is a valid pe dll/exe.. 
      If IsBadReadPtr_(*inth, SizeOf(IMAGE_NT_HEADERS)) = 0  And*inth\Signature = #IMAGE_NT_SIGNATURE 
      ;Good to go! 
      ;Determine the RVA of the exports data directory. 
        exportsStartRVA = *inth\OptionalHeader\DataDirectory[#IMAGE_DIRECTORY_ENTRY_EXPORT]\VirtualAddress 
        exportsEndRVA = exportsStartRVA + *inth\OptionalHeader\DataDirectory[#IMAGE_DIRECTORY_ENTRY_EXPORT]\Size 
        ;Get the first section. 
          *section = *inth + SizeOf(IMAGE_FILE_HEADER)+4 + *inth\FileHeader\SizeOfOptionalHeader 
        ;Identify the section containing the exports.  It is not always .edata ! 
          For i=1 To *inth\FileHeader\NumberOfSections 
            If exportsStartRVA >= *section\VirtualAddress And exportsStartRVA < *section\VirtualAddress + *section\VirtualSize 
              *exsection = *section 
              Break        
            EndIf 
            *section +SizeOf(_IMAGE_SECTION_HEADER) 
          Next 
        If *exsection 
          ;Calculate the difference between the RVA's and the file offsets for the current section. 
            delta = *exsection\VirtualAddress - *exsection\PointerToRawData 
          ;Adjust the exportsStartRVa accordingly to give the file offset of the image export directory, 
          *ExpDir = *idh + exportsStartRVA - delta 
          *pFunctions  = *idh+ *ExpDir\AddressOfFunctions - delta 
          *pNames = *idh+ *ExpDir\AddressOfNames - delta 
          For i = 0 To *ExpDir\NumberOfNames - 1 
            ;Only add the function name if it actually points to some code! 
              entryPointRVA = *pFunctions\l : *pFunctions + SizeOf(LONG) 
              If entryPointRVA <> 0 
                ;Add the name to the strTable() array and increase the count. 
                  If *pNames\l
                    currentsymname$=PeekS(*pNames\l- delta + *idh,-1, #PB_Ascii) 
                    ReDim strTable.s(count) 
                    strTable(count)=currentsymname$ 
                    count+1 
                  EndIf
              EndIf 
            *pNames+SizeOf(LONG)
          Next 
        EndIf        
      EndIf 
    EndIf 
    UnmapViewOfFile_(FileBase) 
    CloseHandle_(hFileMapping) 
    CloseFile(fileid) 
  ProcedureReturn count 
EndProcedure 

Debug "start"

;Test. 
;Dim names.s(0) 
;num=GetDllFunctions("D:\Digital Projects\MAonPC\example_dll.dll", names()) 
;For i = 0 To num-1 
;  Debug names(i) 
;Next 

Debug ""
Debug "link"
#Library = 1

Prototype Hello(int.i)
If OpenLibrary(#Library, "D:\Digital Projects\MAonPC\example_dll.dll")
  
  Hello.Hello = GetFunction(#Library, "say37")
  Debug Hello


  Result = CallFunctionFast(Hello)
  Debug Hello(3)

  CloseLibrary(#Library)
  
Else 
  Debug "failed to open library"
EndIf

; IDE Options = PureBasic 5.21 LTS (Windows - x86)
; CursorPosition = 140
; FirstLine = 92
; Folding = -
; EnableUnicode
; EnableXP
; CompileSourceDirectory