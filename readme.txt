******************************************************************************************************************
About FireFamilyPlus: Version 4.2 Release (2015)   
   
******************************************************************************************************************
FEATURES ADDED in FFP 4.2
******************************************************************************************************************
 
A TERM module (Weather->Term) has been added.  See Help Topic "Term."

New Analysis Variables:

 * Fosberg Fire Weather Index (FFWI). 
 * Live Fuel Index Based Live Herbaceous Fuel Moisture (LFI WoodyFM - Experimental)
 * Live Fuel Index Based Woody Fuel Moisture (LFI HerbFM - Experimental)

New Station Metadata to order more closely align with WIMS Version 2:

 * WIMS Regular Scheduled Observation Time (RS)
 * NESDIS Station ID

 * Solar Radiation and Precipitation Thresholds for computing State of Weather and WetFlag 
    from hourly data.  See Weather->Hourly Data Analysis->Calculated SOW/WetFlag

 * WRCC Station ID

New NFDRS Processing Options to more closely align with WIMS Version 2:

 *  Process NFDRS for both traditional "O" records and WIMS Version 2.0 "R" records with
      calculated State of Weather and WetFlag based on hourly data.   These R records are currently
      only available from WIMS via the PLST or Dashboard (FW9 Export Functions).

 *  WIMS SnowFlag logic in NFDR computations. (The SnowFlag is not yet transmitted to external
      but should be part of FW9 export by fall of 2012.)

******************************************************************************************************************
Fixes and Enhancements
******************************************************************************************************************
Interactive Batch:  More robust; better continuity between working set in Interactive and Classic Modes

NFDRS Calculator:  Validates field limits and recalls last session.

Fire Occurrence Graphs:  File->View Graph Data moved to Option->View Graph Data

Fire Analysis Graphs: Lines thicker and line types differ for Fire-Day types.

Fire Analysis Statistics:  Sum of Squares information removed for easier reading.

Icons added to Toolbar for adding and viewing Fire Business Candidates.

Icons added to Toolbar for viewing Daily (D) and All Observations (R).

Data Count Report:  Reformatted for easier reading.

******************************************************************************************************************
WHO'S RESPONSIBLE?
******************************************************************************************************************

Technical Contact: Larry Bradshaw (lbradshaw@fs.fed.us) at the Rocky Mountain Research 
Station's Fire Sciences Laboratory.

National Support is provided by Fire and Aviation Management, National Information Systems Group, 
USDA, Forest Service -- 

Phone: (866) 224-7677
Email: helpdesk@dms.nwcg.gov

Sponsorship: USDA, Forest Service, Fire and Aviation Management, Washington, DC.

Programming: Stuart Brittain and Luke Schelvan, Systems for Environmental Management, Missoula Montana.

User's Guide and Help System: Deb Tirmenstein, Systems for Environmental Management. 

******************************************************************************************************************



