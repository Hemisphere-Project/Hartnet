#Interval = 10  ;Pause between two getDMX request

;INPUT VALUES
Value = 0
Value_last = 1

;OUTPUT VALUES
Value_send = 0

chanOut = 1

;LOCAL PROCEDURE
Prototype setPORTS(portIN.i, portOUT.i)
Prototype setSUBNET(sub.i)
Prototype startNODE()
Prototype stopNODE()
Prototype restartNODE()
Prototype getDMX(channel.i)
Prototype sendDMX(channel.i, value.i)

;LOAD LIBRARY
#Library = 1
If (OpenLibrary(#Library, "../dll/mgr-artnet-r512.dll")) 
Else
  Debug "failed to open library"
  End
EndIf

;LOAD FUNCTION FROM DLL INTO PUREBASIC PROCEDURE
setPORTS.setPORTS = GetFunction(#Library, "setPORTS") 
setSUBNET.setSUBNET = GetFunction(#Library, "setSUBNET") 
startNODE.startNODE = GetFunction(#Library, "startNODE") 
stopNODE.stopNODE = GetFunction(#Library, "stopNODE")
restartNODE.restartNODE = GetFunction(#Library, "restartNODE")
getDMX.getDMX = GetFunction(#Library, "getDMX") 
sendDMX.sendDMX = GetFunction(#Library, "sendDMX") 


If (startNODE > 0) 
Else 
  Debug "failed to locate startINPUT in the DLL"
  End
EndIf 

If (stopNODE > 0) 
Else 
  Debug "failed to locate stopINPUT in the DLL"
  End
EndIf 

If (getDMX > 0)
Else 
  Debug "failed to locate getDMX function in DLL"
  End
EndIf


;SET PORTS  (INPUT, OUTPUT)  par defaut: 0 / 1
setPORTS(4,6)

;SET SUBNET   par defaut : 0
setSUBNET(0)

;START NODE
startNODE()

    
Repeat   ;EQUIVALENT DE LA MAIN LOOP
      
  Value = getDMX(3)
  If (Value <> Value_last)
    Value_last = Value
    Debug Value
  EndIf
  
  Value_send = Value_send+1
  If (Value_send > 255) 
    Value_send = 0
    chanOut = chanOut + 1
    If (chanOut > 5) 
      chanOut = 1
    EndIf
  EndIf
    
  sendDMX(chanOut, Value_send)
    
  Delay(#Interval)
  
Until #False   ;FIN MAIN LOOP

;STOP NODE
stopNODE()

CloseLibrary(#Library)
    
End  
  


      
 



; IDE Options = PureBasic 5.50 (Windows - x86)
; CursorPosition = 78
; FirstLine = 71
; EnableXP
; Executable = MGRartnet.exe
; EnableUnicode
