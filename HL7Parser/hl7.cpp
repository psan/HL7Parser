#pragma once

#include "stdafx.h"
#include "hl7.h"
#include "patient.h"
#include <iostream>
#include <fstream>

HL7::HL7(string HL7message)
{
	cout << "Building HL7 object..." << endl;
	
		cout << "Loading Message..." << endl;
		fullHL7msg = HL7message;

		cout << "Parsing Segments..." << endl;
		parseSegments(); //encapsulate each HL7 segment into a string

		cout << "Defining Fields..." << endl << endl;
		defineFields(); //Generates multidimensional array with field headers

		cout << "Reporting Segments..." << endl << endl;
		reportSegments(); //sort data from the HL7 message segments into array generated in defineFields()

		cout << "Storing Message Control ID..." << endl << endl;
		messageControlID = MSHfields[8][1]; //stores unique message control ID in HL7 object (for serialization)

		cout << "Building Patient..." << endl << endl;
		buildPatient(); //create an object of the patient class using values from the HL7 message object

		cout << "Checking if message exists..." << endl << endl;
		if (checkDupeMessage()) //if check for existing message in patientDB returns true...
		{
			cout << "Message already exists, not writing to file"; //do nothing
		}
		else //if current HL7 message string not found in db...
		{
			cout << "Storing Patient..." << endl << endl;
			storeMessage(); //store contents of fullHL7message in the db
		}
}

void HL7::defineFields()
{
	cout << "Defining MSH Fields..." << endl << endl;

	//Construct MSH Fields Array
	MSHfields[0][0] = "Field Separator";
	MSHfields[1][0] = "Sending Application";
	MSHfields[2][0] = "Sending Facility";
	MSHfields[3][0] = "Receiving Application";
	MSHfields[4][0] = "Receiving Facility";
	MSHfields[5][0] = "Date/Time of Message";
	MSHfields[6][0] = "Security";
	MSHfields[7][0] = "Message Type";
	MSHfields[8][0] = "Message Control Id";
	MSHfields[9][0] = "Processing Id";
	MSHfields[10][0] = "Version Id";
	MSHfields[11][0] = "Sequence Number";
	MSHfields[12][0] = "Continuation Pointer";
	MSHfields[13][0] = "Accept Acknowledgement Type";
	MSHfields[14][0] = "Application Acknowledgement Type";
	MSHfields[15][0] = "Country Code";
	MSHfields[16][0] = "Character Set";
	MSHfields[17][0] = "Principle Language of Message";
	MSHfields[18][0] = "Sending Facility";

	cout << "Defining PID Fields..." << endl;

	//Construct PID Fields Array
	PIDfields[0][0] = "Set ID - Patient ID";
	PIDfields[1][0] = "Patient ID (External ID)";
	PIDfields[2][0] = "Patient ID (Internal ID)";
	PIDfields[3][0] = "Alternate Patient ID - PID";
	PIDfields[4][0] = "Patient Name";
	PIDfields[5][0] = "Mother's Maiden Name";
	PIDfields[6][0] = "Date/Time of Birth";
	PIDfields[7][0] = "Sex";
	PIDfields[8][0] = "Patient Alias";
	PIDfields[9][0] = "Race";
	PIDfields[10][0] = "Patient Address";
	PIDfields[11][0] = "Country Code";
	PIDfields[12][0] = "Phone Number - Home";
	PIDfields[13][0] = "Phone Number - Business";
	PIDfields[14][0] = "Primary Language";
	PIDfields[15][0] = "Marital Status";
	PIDfields[16][0] = "Religion";
	PIDfields[17][0] = "Patient Account Number";
	PIDfields[18][0] = "SSN Number - Patient";
	PIDfields[19][0] = "Driver's License Number - Patient";
	PIDfields[20][0] = "Mother's Identifier";
	PIDfields[21][0] = "Ethnic Group";
	PIDfields[22][0] = "Birth Place";
	PIDfields[23][0] = "Multiple Birth Indicator";
	PIDfields[24][0] = "Birth Order";
	PIDfields[25][0] = "Citizenship";
	PIDfields[26][0] = "Veterans Millitary Status";
	PIDfields[27][0] = "Nationality";
	PIDfields[28][0] = "Patient Death Date and Time";
	PIDfields[29][0] = "Patient Death Indicator";

	cout << "Defining PV1 Fields..." << endl;

	//Construct PV1 array
	PV1fields[0][0] = "Patient Class";
	PV1fields[1][0] = "Assigned Patient Location";
	PV1fields[2][0] = "Admission Type";
	PV1fields[3][0] = "Preadmit Number";
	PV1fields[4][0] = "Prior Patient Location";
	PV1fields[5][0] = "Attending Doctor";
	PV1fields[6][0] = "Referring Doctor";
	PV1fields[7][0] = "Consulting Doctor";
	PV1fields[8][0] = "Hospital Service";
	PV1fields[9][0] = "Temporary Location";
	PV1fields[10][0] = "Preadmit Test Indicator";
	PV1fields[11][0] = "Readmission Indicator";
	PV1fields[12][0] = "Admit Source";
	PV1fields[13][0] = "Ambulatory Status";
	PV1fields[14][0] = "VIP Indicator";
	PV1fields[15][0] = "Admitting Doctor";
	PV1fields[16][0] = "Patient Type";
	PV1fields[17][0] = "Visit Number";
	PV1fields[18][0] = "Financial Class";
	PV1fields[19][0] = "Charge Price Indicator";
	PV1fields[20][0] = "Courtesy Code";
	PV1fields[21][0] = "Credit Rating";
	PV1fields[22][0] = "Contract Code";
	PV1fields[23][0] = "Contract Effective Date";

	//Construct OBX array
	OBXfields[0][0] = "Set ID - OBX";
	OBXfields[1][0] = "Value Type";
	OBXfields[2][0] = "Observation Identifier";
	OBXfields[3][0] = "Observation Sub-ID";
	OBXfields[4][0] = "Observation Value";
	OBXfields[5][0] = "Units";
	OBXfields[6][0] = "References Range";
	OBXfields[7][0] = "Abnormal FLags";
	OBXfields[8][0] = "Probability";
	OBXfields[9][0] = "Nature of Abnormal Test";
	OBXfields[10][0] = "Observation Result Status";
	OBXfields[11][0] = "Effective Date of Reference Range";
	OBXfields[12][0] = "User Defined Access Checks";
	OBXfields[13][0] = "Date/Time of the Observation";
	OBXfields[14][0] = "Producer's ID";
	OBXfields[15][0] = "Responsible Observer";
	OBXfields[16][0] = "Observation Method";
	OBXfields[17][0] = "Equipment Instance Identifier";
	OBXfields[18][0] = "Date/Time of Analysis";

	//Construct NAT array
	NATfields[0][0] = "Condition";
	NATfields[1][0] = "Medication";
	NATfields[2][0] = "Home State";
	NATfields[3][0] = "Current State";
	NATfields[4][0] = "Exception";
	NATfields[5][0] = "Clinician Instruction";
}

void HL7::parseMSH() //finds the characters "PID" inside the HL7 string, determines the char index, determines MSH segment
{
	pidLocation = fullHL7msg.find("PID");
	cout << "PID segment found at: " << pidLocation << endl;
	MSH = fullHL7msg.substr(0, pidLocation);
}

void HL7::parsePID() //finds the characters "PV1" inside the HL7 string, determines the char index, determines PID segment
{
	pv1Location = fullHL7msg.find("PV1");
	cout << "PV1 segment found at: " << pv1Location << endl;

	PID = fullHL7msg.substr(pidLocation, pv1Location - pidLocation);
}

void HL7::parsePV1() //finds the characters "OBX" inside the HL7 string, determines the char index, determines PV1 segment
{
	obxLocation = fullHL7msg.find("OBX");
	cout << "OBX segment found at: " << obxLocation << endl;
	PV1 = fullHL7msg.substr(pv1Location, obxLocation - pv1Location);
}

void HL7::parseOBX() //defines remaining characters inside the HL7 string as the OBX segment
{
	OBX = fullHL7msg.substr(obxLocation);
}

void HL7::parseSegments()
{
	parseMSH();

	parsePID();

	parsePV1();

	parseOBX();
}


void HL7::getSegmentFields(string segmentString)
{
	int i = 0; //Index of current character
	int n = 0; //MSH field array index

	int leadBarPosition, trailBarPosition;
	bool leadBarSet = false;

	for (char& c : segmentString) //iterate through the message segment string, storing each character in c
	{
		if (c == '|') //if the current character is a bar,
		{
			if (!leadBarSet) //if bar is pidLocation, and no leadBar is defined
			{
				leadBarPosition = i; //set leadBar to current index
				leadBarSet = true; //flag leadbar as true
			}
			else if (leadBarSet) //if bar is pidLocation, and leadBar is defined
			{
				trailBarPosition = i; //set trailBar to current index
				
				if ((segmentString == this->MSH) && (n <= 19)) //if MSH segment, and index n is less than index size of MSHfields array...
				{
					MSHfields[n][1] = segmentString.substr(leadBarPosition + 1, trailBarPosition - leadBarPosition - 1);
					//cout << MSHfields[n][0] << ": " << MSHfields[n][1] << endl;
				}
				
				else if ((segmentString == this->PID) && (n <= 29)) //if PID segment, and index n is less than index size of PIDfields array...
				{
					PIDfields[n][1] = segmentString.substr(leadBarPosition + 1, trailBarPosition - leadBarPosition - 1);
					//cout << PIDfields[n][0] << ": " << PIDfields[n][1] << endl;
				}
				
				else if ((segmentString == this->PV1) && (n <= 24)) //if PV1 segment, and index n is less than index size of PV1fields array...
				{
					PV1fields[n][1] = segmentString.substr(leadBarPosition + 1, trailBarPosition - leadBarPosition - 1);
					//cout << PV1fields[n][0] << ": " << PV1fields[n][1] << endl;
				}
				else if (segmentString == this->OBX && (n <= 19)) //if OBX segment, and index n is less than index size of OBXfields array...
				{
					OBXfields[n][1] = segmentString.substr(leadBarPosition + 1, trailBarPosition - leadBarPosition - 1);
					//cout << OBXfields[n][0] << ": " << OBXfields[n][1] << endl;
				}  
				leadBarPosition = i; //reset the leadbar for the next segment as the trailbar for the previous segment
				n++;
			}
		}
		i++; //iterate to next character
	}
}

void HL7::reportSegments()
{
	getSegmentFields(this->MSH);
	getSegmentFields(this->PID);
	getSegmentFields(this->PV1);
	getSegmentFields(this->OBX);
}

bool HL7::detectPrivateInfo(string segmentArray[]) //accepts pointer to object array as parameter
{
	for (int i = 0; i < 19; i++)
	{
		if (OBXfields[i][1].find("HIV") != string::npos) //if the string "HIV" is found within any of the MSH field values...
		{
			 cout << "Exception condition found, processing..." << endl; 
			 
			 //Start building values for NAT segment
			 NATfields[0][1] = OBXfields[i][1];
			 NATfields[1][1] = "HIV-1 Abs Test";
			 NATfields[2][1] = "NY";
			 NATfields[3][1] = "CT";
			 NATfields[4][1] = "NO";
			 NATfields[5][1] = "None";

			 //build NAT segment
			 NAT = "NAT|" + NATfields[0][1] + "|" + NATfields[1][1] + "|" + NATfields[2][1] + "|" + NATfields[3][1] + "|" + NATfields[4][1] + "|" + NATfields[5][1];
			
			 return true;
		}
	 }
	return false;
}

void HL7::rebuildMessage() //rebuilds fullHL7message with additional segments appended (if necessary)
{
	if (NAT.empty()) //if there's no additional NAT segment
	{
		//do nothing
	}
	else
	{
		fullHL7msg = MSH + "\n\r" + PID + "\n\r" + PV1 + "\n\r" + NAT; //rebuild message with additional segment
	}
}

bool HL7::checkDupeMessage() //check to see if message already exists in database
{
	ifstream messageDB;
	messageDB.open("messageDB.txt"); //open file
	
	for (string line; getline(messageDB, line);) //for each line in messageDB file
	{
		if (line == fullHL7msg) //if the line is identical to fullHL7msg in current HL7 object,
		{
			return true;
			break;
		}

	}
	
	messageDB.close(); //close patientDB file
	
	return false;
}

void HL7::storeMessage() //stores fullHL7msg in patientDB file
{
	ofstream messageDB;
	messageDB.open("messageDB.txt", ofstream::app); //open patientDB file, set write mode to append
	messageDB << fullHL7msg << endl; //write message to patientDB file
	messageDB.close(); //close file
}

void HL7::buildPatient() //creates Patient class object using values from the HL7 object
{
	Patient* testPatient; //create pointer to new Patient object
	testPatient = new Patient(PIDfields[4][1], PIDfields[2][1], PIDfields[6][1], PIDfields[10][1], PIDfields[12][1]); //construct patient object
}

string HL7::printFullMessage()
{
	return fullHL7msg;
}