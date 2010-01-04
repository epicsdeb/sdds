/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

//package SDDS.apps;

import java.io.*;
import java.lang.reflect.*;
import SDDS.java.SDDS.*;
/**
 * This program accesses a series of SDDS files and summarizes the
 * file header for each. It also provides lists of parameters and
 * column.<br><br>
 * usage: java sddsquery [&lt;source-file&gt; ...]<br>
 * [{-parameterlist | -arraylist | -columnlist | version}]<br>
 * [-delimiter=&lt;delimiting-string&gt;]<br>
 * [-appendunits[=bare]]<br>
 * [-sddsOutput=&lt;filename&gt;]<br><br>
 * <table><tr><td>
 * -parameterlist</td><td>
 * The parameter names will be displayed</td></tr><tr><td>
 * -arraylist</td><td>
 * The array names will be displayed</td></tr><tr><td>
 * -columnlist</td><td>
 * The column names will be displayed</td></tr><tr><td>
 * -version</td><td>
 * The file's SDDS version number will be displayed</td></tr><tr><td>
 * -delimiter</td><td>
 * The elements in the output stream will be separated by the delimiter</td></tr><tr><td>
 * -appendunits</td><td>
 * If a column or parameter has units then they will be appended to the output</td></tr><tr><td>
 * -sddsOutput</td><td>
 * Creates an SDDS file containing information about the columns or parameters</td>
 * </table>
 *
 * @author    Robert Soliday 
 *            <a href=mailto:soliday@aps.anl.gov>
 *            soliday@aps.anl.gov</a>
 * @version   1.0 (November 23, 1999)
 * @since     JDK 1.2
 */

class sddsquery {
    static void printUsage() {
	String usage;
	usage = "usage: sddsquery [<source-file> ...]\n";
	usage = usage + "[{-parameterlist | -arraylist | -columnlist | -version}]\n";
	usage = usage + "[-delimiter=<delimiting-string>] [-appendunits[=bare]]\n";
	usage = usage + "[-sddsOutput=<filename>]\n\n";
	usage = usage + "This program accesses a series of SDDS files and\n";
	usage = usage + "summarizes the file header for each. It also provides\n";
	usage = usage + "lists of parameters and columns.\n\n";
	usage = usage + "Program by Robert Soliday.";
	System.err.println(usage);
    }

    public static void main(String[] args) {
	String[][] commandLineArgs;
	String[] inputfile=null;
	String[] parameterNames=null, arrayNames=null, columnNames=null;
	String[] units=null, symbol=null, format=null, description=null;
	String delimiter = "\n";
	String outputFile = null;
	int[] type, field_length;
	int arguments, elements, i, j, k, n, files=0;
	boolean parameterList = false;
	boolean arrayList = false;
	boolean columnList = false;
	boolean version = false;
	boolean appendUnits = false;
	boolean bare = false;
	BufferedOutputStream bos = new BufferedOutputStream(System.err, 4096 /* buffsize */);
	PrintWriter pw = new PrintWriter(bos, false /* auto flush */);
	
	commandLineArgs = SDDSUtil.parseCommandLine(args);
	arguments = Array.getLength(commandLineArgs);
	if (arguments==0) {
	  printUsage();
	  return;
	}
	for (i=0;i<arguments;i++) {
	    elements = Array.getLength(commandLineArgs[i]);
	    if (elements == 0)
		continue;
	    if (SDDSUtil.compare(commandLineArgs[i][0],"-parameterlist")) {
		parameterList = true;
	    } else if (SDDSUtil.compare(commandLineArgs[i][0],"-arraylist")) {
		arrayList = true;
	    } else if (SDDSUtil.compare(commandLineArgs[i][0],"-columnlist")) {
		columnList = true;
	    } else if (SDDSUtil.compare(commandLineArgs[i][0],"-version")) {
		version = true;
	    } else if (SDDSUtil.compare(commandLineArgs[i][0],"-delimiter")) {
		if (elements != 2) {
		    System.err.println("invalid command line argument: " + args[i]);
		    printUsage();
		    return;
		}
		delimiter = commandLineArgs[i][1];
	    } else if (SDDSUtil.compare(commandLineArgs[i][0],"-appendunits")) {
		if (elements > 2) {
		    System.err.println("invalid command line argument: " + args[i]);
		    printUsage();
		    return;
		}
		appendUnits = true;
		if (elements == 2) {
		    if (SDDSUtil.compare(commandLineArgs[i][1],"bare"))
			bare = true;
		}
	    } else if (SDDSUtil.compare(commandLineArgs[i][0],"-sddsOutput")) {
		if (elements != 2) {
		    System.err.println("invalid command line argument: " + args[i]);
		    printUsage();
		    return;
		}
		outputFile = commandLineArgs[i][1];
	    } else {
		if ((elements != 1) || (commandLineArgs[i][0].charAt(0) == '-')) {
		    System.err.println("invalid command line argument: " + args[i]);
		    printUsage();
		    return;
		}
		files++;
		if (inputfile == null)
		    inputfile = new String[1];
		else
		    inputfile = (String[])SDDSUtil.resize(inputfile,files);
		inputfile[files-1] = commandLineArgs[i][0];
	    }
	}
	if (inputfile == null) {
	    System.err.println("missing SDDS file name");
	    printUsage();
	    return;
	}
	
	SDDSFile sdds, sddsout=null;
	if (outputFile != null) {
	    sddsout = new SDDSFile(outputFile);
	    String[] parNames = {"Class","Filename"};
	    String[] parTypes = {"string","string"};
	    if (!sddsout.defineParameters(parNames,parTypes)) {
		sddsout.printErrors();
		return;
	    }
	    String[] colNames = {"Name","Units","Symbol","Format","Type","Description"};
	    String[] colTypes = {"string","string","string","string","string","string"};
	    if (!sddsout.defineColumns(colNames,colTypes)) {
		sddsout.printErrors();
		return;
	    }
	}
	for (i=0;i<files;i++) {
	    sdds = new SDDSFile(inputfile[i]);
	    if (!sdds.openInputFile()) {
		sdds.printErrors();
		return;
	    }
	    if (!sdds.parseHeaders()) {
		sdds.printErrors();
		return;
	    }
	    if (parameterList) {
		parameterNames = sdds.getParameterNames();
		units = sdds.getAllParameterUnits();
		if (outputFile != null) {
		    if (!sddsout.setParameter("Filename", inputfile[i], i+1)) {
			sddsout.printErrors();
			return;
		    }
		    if (!sddsout.setParameter("Class", "parameter", i+1)) {
			sddsout.printErrors();
			return;
		    }	
		    if (parameterNames != null) {
			symbol = sdds.getParameterSymbols();
			format = sdds.getParameterFormatStrings();
			type = sdds.getParameterTypes();
			description = sdds.getParameterDescriptions();
			for (j=0;j<Array.getLength(parameterNames);j++) {
			    if (!sddsout.setColumnElement("Name", parameterNames[j],i+1,j+1)) {
				sddsout.printErrors();
				return;
			    }	
			    if (!sddsout.setColumnElement("Units", units[j],i+1,j+1)) {
				sddsout.printErrors();
				return;
			    }	
			    if (!sddsout.setColumnElement("Symbol", symbol[j],i+1,j+1)) {
				sddsout.printErrors();
				return;
			    }	
			    if (!sddsout.setColumnElement("Format", format[j],i+1,j+1)) {
				sddsout.printErrors();
				return;
			    }	
			    if (!sddsout.setColumnElement("Type", SDDSUtil.getTypeName(type[j]),i+1,j+1)) {
				sddsout.printErrors();
				return;
			    }	
			    if (!sddsout.setColumnElement("Description", description[j],i+1,j+1)) {
				sddsout.printErrors();
				return;
			    }	
			}
		    }
		} else if (parameterNames != null) {
		    for (j=0;j<Array.getLength(parameterNames);j++) {
			pw.print(parameterNames[j]);
			if (units[j] != null) {
			    if (appendUnits) {
				if (bare)
				    pw.print(" " + units[j]);
				else
				    pw.print(" (" + units[j] + ")");
			    }
			}
			pw.print(delimiter);
		    }
		}
	    } else if (arrayList) {
		arrayNames = sdds.getArrayNames();
		units = sdds.getAllArrayUnits();
		if (outputFile != null) {
		} else if (arrayNames != null) {
		    for (j=0;j<Array.getLength(arrayNames);j++) {
			pw.print(arrayNames[j]);
			if (units[j] != null) {
			    if (appendUnits) {
				if (bare)
				    pw.print(" " + units[j]);
				else
				    pw.print(" (" + units[j] + ")");
			    }
			}
			pw.print(delimiter);
		    }
		}
	    } else if (columnList) {
System.err.println("1");
		columnNames = sdds.getColumnNames();
		units = sdds.getAllColumnUnits();
		if (outputFile != null) {
		    if (!sddsout.setParameter("Filename", inputfile[i], i+1)) {
			sddsout.printErrors();
			return;
		    }
		    if (!sddsout.setParameter("Class", "column", i+1)) {
			sddsout.printErrors();
			return;
		    }	
		    if (columnNames != null) {
			symbol = sdds.getColumnSymbols();
			format = sdds.getColumnFormatStrings();
			type = sdds.getColumnTypes();
			description = sdds.getColumnDescriptions();
			for (j=0;j<Array.getLength(columnNames);j++) {
			    if (!sddsout.setColumnElement("Name", columnNames[j],i+1,j+1)) {
				sddsout.printErrors();
				return;
			    }	
			    if (!sddsout.setColumnElement("Units", units[j],i+1,j+1)) {
				sddsout.printErrors();
				return;
			    }	
			    if (!sddsout.setColumnElement("Symbol", symbol[j],i+1,j+1)) {
				sddsout.printErrors();
				return;
			    }	
			    if (!sddsout.setColumnElement("Format", format[j],i+1,j+1)) {
				sddsout.printErrors();
				return;
			    }	
			    if (!sddsout.setColumnElement("Type", SDDSUtil.getTypeName(type[j]),i+1,j+1)) {
				sddsout.printErrors();
				return;
			    }	
			    if (!sddsout.setColumnElement("Description", description[j],i+1,j+1)) {
				sddsout.printErrors();
				return;
			    }	
			}
		    }
		} else if (columnNames != null) {
		    for (j=0;j<Array.getLength(columnNames);j++) {
			pw.print(columnNames[j]);
			if (units[j] != null) {
			    if (appendUnits) {
				if (bare)
				    pw.print(" " + units[j]);
				else
				    pw.print(" (" + units[j] + ")");
			    }
			}
			pw.print(delimiter);
		    }
		}
	    } else if (version) {
		pw.print(sdds.layoutVersion + delimiter);
	    } else {
		pw.println("\nfile " + inputfile[i] + " is in SDDS protocol version " + sdds.layoutVersion);
		if (sdds.descriptionText != null)
		    pw.println("description: " + sdds.descriptionText);
		if (sdds.descriptionContents != null)
		    pw.println("contents: " + sdds.descriptionContents);
		if (sdds.asciiFile)
		    pw.println("\ndata is ASCII");
		else
		    pw.println("\ndata is binary");
		columnNames = sdds.getColumnNames();
		if (columnNames != null) {
		    units = sdds.getAllColumnUnits();
		    symbol = sdds.getColumnSymbols();
		    format = sdds.getColumnFormatStrings();
		    type = sdds.getColumnTypes();
		    description = sdds.getColumnDescriptions();
		    field_length = sdds.getColumnFieldLengths();
		    pw.println("\n" + Array.getLength(columnNames) + " columns of data:");
		    pw.println("NAME            UNITS           SYMBOL          FORMAT          TYPE      FIELD  DESCRIPTION");
		    pw.println("                                                                          LENGTH");
		    for (j=0;j<Array.getLength(columnNames);j++) {
			pw.println(appendSpaces(columnNames[j],15) + " " + appendSpaces(units[j],15) + " " + appendSpaces(symbol[j],15) + " " + appendSpaces(format[j],15) + " " + appendSpaces(SDDSUtil.getTypeName(type[j]),9) + " " + appendSpaces(Integer.toString(field_length[j]),6) + " " + appendSpaces(description[j],0));
		    }
		}
		parameterNames = sdds.getParameterNames();
		if (parameterNames != null) {
		    units = sdds.getAllParameterUnits();
		    symbol = sdds.getParameterSymbols();
		    type = sdds.getParameterTypes();
		    description = sdds.getParameterDescriptions();
		    pw.println("\n" + Array.getLength(parameterNames) + " parameters:");
		    pw.println("NAME                UNITS               SYMBOL              TYPE                DESCRIPTION");
		    for (j=0;j<Array.getLength(parameterNames);j++) {
			pw.println(appendSpaces(parameterNames[j],19) + " " + appendSpaces(units[j],19) + " " + appendSpaces(symbol[j],19) + " " + appendSpaces(SDDSUtil.getTypeName(type[j]),19) + " " + appendSpaces(description[j],0));
		    }
		}




	    }
	    if (outputFile == null) {
		pw.flush();
		pw.close();
	    }
	    if (!sdds.closeInput()) {
		sdds.printErrors();
		return;
	    }
	}
	if (outputFile != null) {
	    if (!sddsout.writeFile()) {
		sddsout.printErrors();
		return;
	    }
	}
    }
    
    private static String appendSpaces(String s, int length) {
	int i;
	if (s == null)
	    s = "NULL";
	for (i = s.length();i<length;i++) {
	    s = s + " ";
	}
	return s;
    }
}
