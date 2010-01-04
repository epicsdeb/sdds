/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

package SDDS.java.SDDS;

import java.io.*;
import java.lang.reflect.*;
import java.util.StringTokenizer;
import java.util.zip.*;
import java.net.*;

/**
 * The SDDS File object is used to read and write Self Describing
 * Data Sets. It creates and manages SDDS Parameter objects and 
 * SDDS Column objects. A brief introduction to the SDDS concept can be found 
 * <a href=http://www.aps.anl.gov/asd/oag/SDDSIntroTalk/slides.html>here</a>.
 *
 * @author    Robert Soliday <a href=mailto:soliday@aps.anl.gov>soliday@aps.anl.gov</a>
 * @version   1.0 (November 1, 1999)
 * @since JDK 1.2
 *
 * @upgrade   Seth Nemesure <a href=mailto:seth@bnl.gov>seth@bnl.gov</a>
 * @version   1.1 (November, 2001)
 * @since JDK 1.3
 */
public class SDDSFile {
    FileOutputStream fos;
    FileInputStream fis;
    InputStream is;
    BufferedOutputStream bos;
    BufferedInputStream bis;
    GZIPInputStream gzis;
    GZIPOutputStream gzos;
    DataOutputStream dos;
    DataInputStream dis;
    LEDataOutputStream ledos;
    LEDataInputStream ledis;
    public String fileName;
    public URL URLName;
    public String descriptionText = null;
    public String descriptionContents = null;
    int parameterCount = 0;
    SDDSParameter[] parameterObject;
    int columnCount = 0;
    SDDSColumn[] columnObject;
    int arrayCount = 0;
    SDDSArray[] arrayObject;
    int n_errors;
    String[] error_description;
    final int SDDS_SHORT = 4;
    final int SDDS_LONG = 3;
    final int SDDS_FLOAT = 2;
    final int SDDS_DOUBLE = 1;
    final int SDDS_STRING = 5;
    final int SDDS_CHARACTER = 6;
    public boolean asciiFile;
    public boolean noRowCount;
    public boolean bigEndian;
    public int layoutVersion;
    public boolean isURL = false;
    /**
     * Create a new SDDS file that can be used to read and write SDDS files.
     */
    public SDDSFile() {
	String os = System.getProperty("os.name");
	n_errors = 0;
	asciiFile = true;
	noRowCount = false;
	if (os.equals("Linux") || os.startsWith("Windows")) {
	    bigEndian = false;
	} else {
	    bigEndian = true;
	}
    }

    /**
     * Create a new SDDS file that can be used to read and write SDDS files.
     * 
     * @param filename      file name to read or write data
     */
    public SDDSFile(String filename) {
	String os = System.getProperty("os.name");
        fileName = filename;
	n_errors = 0;
	asciiFile = true;
	noRowCount = false;
	if (os.equals("Linux") || os.startsWith("Windows")) {
	    bigEndian = false;
	} else {
	    bigEndian = true;
	}
    }

    /**
     * Create a new SDDS file that can be used to read and write SDDS files.
     * 
     * @param filename      file name to read or write data
     */
    public SDDSFile(URL filename) {
	String os = System.getProperty("os.name");
        fileName = filename.getFile();
	URLName = filename;
	n_errors = 0;
	asciiFile = true;
	noRowCount = false;
	isURL = true;
	if (os.equals("Linux") || os.startsWith("Windows")) {
	    bigEndian = false;
	} else {
	    bigEndian = true;
	}
    }

    /**
     * Create a new SDDS file that can be used to read and write SDDS files.
     * 
     * @param binary         <code>true</code> if binary output, <code>false</code> if ascii output
     */
    public SDDSFile(boolean binary) {
	String os = System.getProperty("os.name");
	n_errors = 0;
	asciiFile = !binary;
	noRowCount = false;
	if (os.equals("Linux") || os.startsWith("Windows")) {
	    bigEndian = false;
	} else {
	    bigEndian = true;
	}
    }

    /**
     * Create a new SDDS file that can be used to read and write SDDS files.
     * 
     * @param filename      file name to read or write data
     * @param binary         <code>true</code> if binary output, <code>false</code> if ascii output
     */
    public SDDSFile(String filename, boolean binary) {
	String os = System.getProperty("os.name");
        fileName = filename;
	n_errors = 0;
	asciiFile = !binary;
	noRowCount = false;
	if (os.equals("Linux") || os.startsWith("Windows")) {
	    bigEndian = false;
	} else {
	    bigEndian = true;
	}
    }

    /**
     * Set the file name for input and output.
     * 
     * @param filename      file name to read or write data
     *
     * @return        <code>true</code> if successful<br>
     *                <code>false</code> if an error occurs
     */
    public boolean setFileName(String filename) {
	fileName = filename;
	return true;
    }

    /**
     * Set binary mode for accessing files.
     * 
     * @param binary         <code>true</code> if binary output, <code>false</code> if ascii output
     */
    public void setMode(boolean binary) {
	asciiFile = !binary;
    }

    /**
     * Set endian mode for accessing files.
     * 
     * @param little_endian  <code>true</code> if little endian input and output, 
     *                       <code>false</code> if big endian input and output. Not used with ascii files.
     */
    public void setEndian(boolean little_endian) {
	bigEndian = !little_endian;
    }

    /**
     * Set for fixed number of rows in column data or dynamic.  This method will have no effect
     * if called after header is written.
     *
     * @param norowcount <code>true</code> dynamic column data
     *                   <code>false</code> fixed number of rows in column data
     *
     * @return           <code>true</code> if successful <br>
     *                   <code>false</code> if an error occurs
     */
    public boolean setNoRowCount(boolean norowcount) 
    {
	noRowCount = norowcount;
	return true;
    }

    /**
     * Switch writing of column data from one file to a new file without having to redefine parameters,
     * arrays and columns.  This is useful for when noRowCount is set and the number of rows has grown
     * rather large and continuing the data in a new file would seem appropriate.  The parameter and array data
     * is duplicated in the new file and the column data headers are duplicated in the new file
     * without carrying the actual column data from the old file over. Fresh row data can be added one
     * row at a time to the new file.
     *
     * @param fileName    <code>fileName</code>  The new file to be written into
     *
     * @return            <code>true</code>   If success<br>
     *                    <code>false</code>  If an error occurs
     */
    public boolean switchToNewFile(String fileName)
    {
	/* delete the row data but keep the row definitions and all arrays and parameters */
	noRowCount = true;
	int rowCount;
	for (int page = 1; page <= pageCount(); page++){
	    rowCount = getRowCount(page);
	    for (int row = 1; row <= rowCount; row++){
		deleteRow(page, row);
	    }
	}
	/* set new file name */
	setFileName(fileName);
	return writeFile();
    }

    /**
     * Duplicate an SDDSFile object copying over appropriate header information and
     * data.
     *
     * @param file          SDDSFile object to be copied<br>
     * @param parameters    <code>true</code> copy parameter information<br>
     *                      <code>false</code> ignore parameter data
     * @param arrays        <code>true</code> copy array information<br>
     *                      <code>false</code> ignore array data
     * @param columns       <code>true</code> copy column information<br>
     *                      <code>false</code> ignore column data
     *
     * @return              <code>true</code> if successful<br>
     *                      <code>false</code> if error occurs
     */
    public boolean duplicateCopy(SDDSFile file, boolean parameters, boolean arrays, boolean columns)
    {
	Object object;
	Object objects[];
	int type, pcount, size;
	// parameters
	for (int i=0; parameters && i<file.parameterCount; i++){
	    type = file.parameterObject[i].getType();
	    pcount = file.parameterObject[i].getPageCount();
	    if (defineParameter(file.parameterObject[i].getName(), file.parameterObject[i].getSymbol(),
				file.parameterObject[i].getUnits(), file.parameterObject[i].getDescription(),
				file.parameterObject[i].getFormat_String(), type,
				file.parameterObject[i].getFixed_Value()) == -1)
		return false;
	    for (int page=1; page<=pcount; page++) {
		object = file.parameterObject[i].getValue(page, false);
		switch(type){
		case 1: // double
		    if (!setParameter(i, ((Double)object).doubleValue(), page))
			return false;
		    break;
		case 2: // float
		    if (!setParameter(i, ((Float)object).floatValue(), page))
			return false;
		    break;
		case 3: // long
		    if (!setParameter(i, ((Long)object).longValue(), page))
			return false;
		    break;
		case 4: // short
		    if (!setParameter(i, ((Short)object).shortValue(), page))
			return false;
		    break;
		case 5: // string
		case 6: // character
		    if (!setParameter(i, object.toString(), page))
			return false;
		    break;
		}
	    }
	}

	// arrays
	for (int i=0; arrays && i<file.arrayCount; i++){
	    type = file.arrayObject[i].getType();
	    pcount = file.arrayObject[i].getPageCount();
	    if (defineArray(file.arrayObject[i].getName(), file.arrayObject[i].getSymbol(),
			    file.arrayObject[i].getUnits(), file.arrayObject[i].getDescription(),
			    file.arrayObject[i].getFormat_String(), file.arrayObject[i].getGroup_Name(),
			    file.arrayObject[i].getType(), 0, file.arrayObject[i].getDimensions()) == -1)
		return false;
	    for (int page=1; page<=pcount; page++) {
		setArrayDim(page,i,file.arrayObject[i].getDim(page));
		objects = file.arrayObject[i].getValues(page, false);
		size = Array.getLength(objects);
		switch(type){
		case 1: // double
		    {
			double[] values = new double[size];
			for (int j=0;j<size;j++)
			    values[j] = ((Double)objects[j]).doubleValue();
			if (!setArray(i, values, page))
			    return false;
			break;
		    }
		case 2: // float
		{
		    float[] values = new float[size];
		    for (int j=0;j<size;j++)
			values[j] = ((Float)objects[j]).floatValue();
		    if (!setArray(i, values, page))
			return false;
		    break;
		}
		case 3: // long
		    {
			long[] values = new long[size];
			for (int j=0;j<size;j++)
			    values[j] = ((Long)objects[j]).longValue();
			if (!setArray(i, values, page))
			    return false;
			break;
		    }
		case 4: // short
		    {
			short[] values = new short[size];
			for (int j=0;j<size;j++)
			    values[j] = ((Short)objects[j]).shortValue();
			if (!setArray(i, values, page))
			    return false;
			break;
		    }
		case 5: // string
		case 6: // character
		    {
			String[] values = new String[size];
			for (int j=0;j<size;j++)
			    values[j] = objects[j].toString();
			if (!setArray(i, values, page))
			    return false;
			break;
		    }
		}
	    }
	}

	// columns
	for (int i=0; columns && i<file.columnCount; i++){
	    type = file.columnObject[i].getType();
	    pcount = file.columnObject[i].getPageCount();
	    if (defineColumn(file.columnObject[i].getName(), file.columnObject[i].getSymbol(),
			     file.columnObject[i].getUnits(), file.columnObject[i].getDescription(),
			     file.columnObject[i].getFormat_String(), file.columnObject[i].getType(),
			     file.columnObject[i].getField_Length()) == -1)
		return false;
	    for (int page=1; page<=pcount; page++) {
		objects = file.columnObject[i].getValues(page, false);
		size = Array.getLength(objects);
		switch(type){
		case 1: // double
		    {
			double[] values = new double[size];
			for (int j=0;j<size;j++)
			    values[j] = ((Double)objects[j]).doubleValue();
			if (!setColumn(i, values, page))
			    return false;
			break;
		    }
		case 2: // float
		    {
			float[] values = new float[size];
			for (int j=0;j<size;j++)
			    values[j] = ((Float)objects[j]).floatValue();
			if (!setColumn(i, values, page))
			    return false;
			break;
		    }
		case 3: // long
		    {
			long[] values = new long[size];
			for (int j=0;j<size;j++)
			    values[j] = ((Long)objects[j]).longValue();
			if (!setColumn(i, values, page))
			    return false;
			break;
		    }
		case 4: // short
		    {
			short[] values = new short[size];
			for (int j=0;j<size;j++)
			    values[j] = ((Short)objects[j]).shortValue();
			if (!setColumn(i, values, page))
			    return false;
			break;
		    }
		case 5: // string
		case 6: // character
		    {
			String[] values = new String[size];
			for (int j=0;j<size;j++)
			    values[j] = objects[j].toString();
			if (!setColumn(i, values, page))
			    return false;
			break;
		    }
		}
	    }
	}
      
	return true;
    }

    /**
     * Open a file for writting.
     * 
     * @return              <code>true</code> if successful<br>
     *                      <code>false</code> if error occurs
     */
    public boolean openOutputFile() {
	if (fileName == null) {
	    bos = new BufferedOutputStream(System.out, 4096 /* buffsize */);
	    dos = new DataOutputStream(bos);
	    ledos = new LEDataOutputStream(bos);
	    
	} else {
	    try {
		fos = new FileOutputStream(fileName, false /* append */);
	    } catch(IOException e) {
		setError("problem opening output file (openOutputFile)");
		return false;
	    }
	    if (fileName.endsWith(".gz")) {
		try {
		    gzos = new GZIPOutputStream(fos, 4096 /* buffsize */);
		} catch (Exception e) {
		    setError("unable to open file in gzip format (openOutputFile)");
		    return false;
		} 
		dos = new DataOutputStream(gzos);
		ledos = new LEDataOutputStream(gzos);
	    } else {
		bos = new BufferedOutputStream(fos, 4096 /* buffsize */);
		dos = new DataOutputStream(bos);
		ledos = new LEDataOutputStream(bos);
	    }
	}
	return true;       
    }

    /**
     * Write out the SDDS file
     *
     * @return             <code>true</code> if successful<br>
     *                     <code>false</code> if an error occurs
     */
    public boolean writeFile() {
	if (!openOutputFile()) {
	    return false;
	}
        if (!writeLayout()) {
	    closeOutput();
	    return false;
	}
	if (!writePages()) {
	    closeOutput();
	    return false;
	}
	if (!noRowCount) {
	    if (!closeOutput()) {
		return false;
	    }
	}
	return true;
    }

    /**
     * Read the SDDS file
     *
     * @return            <code>true</code> if successful<br>
     *                    <code>false</code> if an error occurs
     */
    public boolean readFile() {
	if (!openInputFile()) {
	    return false;
	}
	if (!parseHeaders()) {
	    closeInput();
	    return false;
	}
	if (asciiFile) {
	    if (!readAsciiData()) {
		closeInput();
		return false;
	    }
	} else {
	    if (!readBinaryData()) {
		closeInput();
		return false;
	    }
	}
	if (!closeInput())
	    return false;
	return true;
    }

    /**
     * Open SDDS input file
     *
     * @return            <code>true</code> if successful<br>
     *                    <code>false</code> if an error occurs
     */
    public boolean openInputFile() {
	if (fileName == null) {
	    bis = new BufferedInputStream(System.in, 4096 /* buffsize */);
	    dis = new DataInputStream(bis);
	    ledis = new LEDataInputStream(bis);
	} else {
	    try {
		if (isURL) {
		    is = URLName.openStream();
		} else {
		    fis = new FileInputStream(fileName);
		}
	    } catch (Exception e) {
		setError("input file does not exist (openInputFile)");
		return false;
	    }
	    if (fileName.endsWith(".gz")) {
		try {
		    if (isURL) {
			gzis = new GZIPInputStream(is, 4096 /* buffsize */);
		    } else {
			gzis = new GZIPInputStream(fis, 4096 /* buffsize */);
		    }
		} catch (Exception e) {
		    setError("input file has an invalid gzip format (openInputFile)");
		    return false;
		} 
		dis = new DataInputStream(gzis);
		ledis = new LEDataInputStream(gzis);
	    } else {
		if (isURL) {
		    bis = new BufferedInputStream(is, 4096 /* buffsize */);
		} else {
		    bis = new BufferedInputStream(fis, 4096 /* buffsize */);
		}
		dis = new DataInputStream(bis);
		ledis = new LEDataInputStream(bis);
	    }
	}
	return true;
    }

    /**
     * Parse SDDS input file header lines
     *
     * @return            <code>true</code> if successful<br>
     *                    <code>false</code> if an error occurs
     */
    public boolean parseHeaders() {
	StreamTokenizer st;
	StringReader sr;
	final int quoteChar = 34;
	int header = 0;
	String name, symbol, units, description, format_string, group_name, fixed_value;
	String associateName, associateFilename, associatePath, associateSdds;
	int type, field_length, dimensions;
	String token, line;
	int c;
	
	arrayCount = parameterCount = columnCount = 0;
	name = symbol = units = description = format_string = group_name = fixed_value = null;
	type = field_length = dimensions = 0;
	associateName = associateFilename = associatePath = associateSdds = null;
	
	try {
	    try {
		sr = new StringReader(dis.readLine());
	    } catch (Exception e) {
		setError("unable to read header lines (parseHeaders)");
		return false;
	    }
	    st = new StreamTokenizer(sr);
	    st.resetSyntax();
	    st.whitespaceChars('\u0000','\u0020');
	    st.commentChar('!');
	    st.quoteChar(34);
	    st.wordChars(35,255);
	    st.whitespaceChars('=','=');
	    st.whitespaceChars(',',',');
	    st.eolIsSignificant(false);
	    st.lowerCaseMode(false);

	    c = st.nextToken();
	    if (c != StreamTokenizer.TT_WORD) {
		setError("Missing SDDS version number (parseHeaders)");
		return false;
	    }
	    token = st.sval;
	    if (!SDDSUtil.strncmp(token,"SDDS",4)) {
		setError("Missing SDDS version number (parseHeaders)");
		return false;
	    }
	    try {
		layoutVersion = (Integer.valueOf(SDDSUtil.strchop(token,4))).intValue();
	    } catch (NumberFormatException e) {
		setError("unsupported SDDS file version (parseHeaders)");
		return false;
	    }
	outer:
	    while (true) {
		
		c = st.nextToken();
		if ((c != StreamTokenizer.TT_WORD) && (c != quoteChar)) {		
		    try {
			line = new String(dis.readLine());
			sr = new StringReader(line);
		    } catch (Exception e) {
			setError("unable to read header lines (parseHeaders)");
			return false;
		    }
		    st = new StreamTokenizer(sr);
		    st.resetSyntax();
		    st.whitespaceChars('\u0000','\u0020');
		    st.commentChar('!');
		    st.quoteChar(34);
		    st.wordChars(35,255);
		    st.whitespaceChars('=','=');
		    st.whitespaceChars(',',',');
		    st.eolIsSignificant(false);
		    st.lowerCaseMode(false);
		    
		    c = st.nextToken();
		    if ((c != StreamTokenizer.TT_WORD) && (c != quoteChar)) {
			if (SDDSUtil.strncmp(line,"!# big-endian",13)) {
			    bigEndian = true;
			} else if (SDDSUtil.strncmp(line,"!# little-endian",16)) {
			    bigEndian = false;
			}
			continue;
		    }
		}

		token = st.sval;
		if (header == 0) {
		    if (token.equals("&parameter")) {
			name = symbol = units = description = format_string = fixed_value = null;
			type = 0;
			header = 1;
		    } else if (token.equals("&column")) {
			name = symbol = units = description = format_string = null;
			type = field_length = 0;
			header = 2;
		    } else if (token.equals("&data")) {
			header = 3;
		    } else if (token.equals("&description")) {
		        header = 4;
		    } else if (token.equals("&array")) {
		        name = symbol = units = description = format_string = group_name = null;
			type = field_length = 0;
			dimensions = 1;
		        header = 5;
		    } else if (token.equals("&associate")){
			// this is not supported
			header = 6;
		    } else {
			break outer;
		    }
		    continue;
		}
		switch (header) {
		case 1:
		    if (token.equals("&end")) {
			header = 0;
			if (defineParameter(name, symbol, units, description, format_string, type, fixed_value) == -1) {
			    setError("invalid parameter header (parseHeaders)");
			    return false;
			}
			header = 0;
			break;
		    }
		    if (token.equals("name")) {
			c = st.nextToken();
			if ((c != StreamTokenizer.TT_WORD) && (c != quoteChar))
			    break outer;
			name = st.sval;
		    } else if (token.equals("type")) {
			c = st.nextToken();
			if ((c != StreamTokenizer.TT_WORD) && (c != quoteChar))
			    break outer;
			type = SDDSUtil.identifyType(st.sval);
		    } else if (token.equals("symbol")) {
			c = st.nextToken();
			if ((c != StreamTokenizer.TT_WORD) && (c != quoteChar))
			    break outer;
			symbol = st.sval;
		    } else if (token.equals("units")) {
			c = st.nextToken();
			if ((c != StreamTokenizer.TT_WORD) && (c != quoteChar))
			    break outer;
			units = st.sval;
		    } else if (token.equals("description")) {
			c = st.nextToken();
			if ((c != StreamTokenizer.TT_WORD) && (c != quoteChar))
			    break outer;
			description = st.sval;
		    } else if (token.equals("format_string")) {
			c = st.nextToken();
			if ((c != StreamTokenizer.TT_WORD) && (c != quoteChar))
			    break outer;
			format_string = st.sval;
		    } else if (token.equals("fixed_value")) {
			c = st.nextToken();
			if ((c != StreamTokenizer.TT_WORD) && (c != quoteChar))
			    break outer;
			fixed_value = st.sval;
		    }
		    break;
		case 2:
		    if (token.equals("&end")) {
			if (defineColumn(name, symbol, units, description, format_string, type, field_length) == -1) {
			    setError("invalid column header (parseHeaders)");
			    return false;
			}
			header = 0;
		    }
		    if (token.equals("name")) {
			c = st.nextToken();
			if ((c != StreamTokenizer.TT_WORD) && (c != quoteChar))
			    break outer;
			name = st.sval;
		    } else if (token.equals("type")) {
			c = st.nextToken();
			if ((c != StreamTokenizer.TT_WORD) && (c != quoteChar))
			    break outer;
			type = SDDSUtil.identifyType(st.sval);
		    } else if (token.equals("symbol")) {
			c = st.nextToken();
			if ((c != StreamTokenizer.TT_WORD) && (c != quoteChar))
			    break outer;
			symbol = st.sval;
		    } else if (token.equals("units")) {
			c = st.nextToken();
			if ((c != StreamTokenizer.TT_WORD) && (c != quoteChar))
			    break outer;
			units = st.sval;
		    } else if (token.equals("description")) {
			c = st.nextToken();
			if ((c != StreamTokenizer.TT_WORD) && (c != quoteChar))
			    break outer;
			description = st.sval;
		    } else if (token.equals("format_string")) {
			c = st.nextToken();
			if ((c != StreamTokenizer.TT_WORD) && (c != quoteChar))
			    break outer;
			format_string = st.sval;
		    } else if (token.equals("field_length")) {
			c = st.nextToken();
			if ((c != StreamTokenizer.TT_WORD) && (c != quoteChar))
			    break outer;
			try {
			    field_length = (Integer.valueOf(st.sval)).intValue();
			} catch (NumberFormatException e) {
			    setError("invalid field_length (parseHeaders)");
			    return false;
			}
		    }
		    break;
		case 3:
		    if (token.equals("&end")) {
			return true;
		    }
		    if (token.equals("mode")) {
			c = st.nextToken();
			if ((c != StreamTokenizer.TT_WORD) && (c != quoteChar))
			    break outer;
			if (st.sval.equals("binary"))
			    asciiFile = false;
			else
			    asciiFile = true;
		    }
		    if (token.equals("no_row_counts")){
			c = st.nextToken();
			if ((c != StreamTokenizer.TT_WORD) && (c != quoteChar))
			    break outer;
			try {
			    if ((Integer.valueOf(st.sval)).intValue() != 0)
				noRowCount = true;
			    else
				noRowCount = false;
			} catch (NumberFormatException e) {
			    setError("invalid no_row_counts (parseHeaders)");
			    return false;
			}
		    }
		    if (token.equals("endian")) {
			c = st.nextToken();
			if ((c != StreamTokenizer.TT_WORD) && (c != quoteChar))
			    break outer;
			if (st.sval.equals("little"))
			    bigEndian = false;
			else
			    bigEndian = true;
		    }
		    break;
		case 4:
		    if (token.equals("&end")) {
			header = 0;
			break;
		    }
		    if (token.equals("text")) {
			c = st.nextToken();
			if ((c != StreamTokenizer.TT_WORD) && (c != quoteChar))
			    break outer;
			descriptionText = st.sval;
		    } else if (token.equals("contents")) {
			c = st.nextToken();
			if ((c != StreamTokenizer.TT_WORD) && (c != quoteChar))
			    break outer;
			descriptionContents = st.sval;
		    }
		    break;
		case 5:
		    if (token.equals("&end")) {
			if (defineArray(name, symbol, units, description, format_string, 
					group_name, type, field_length, dimensions) == -1){
			    setError("invalid array header (parseHeaders)");
			    return false;
			}
			header = 0;
		    }
		    if (token.equals("name")) {
			c = st.nextToken();
			if ((c != StreamTokenizer.TT_WORD) && (c != quoteChar))
			    break outer;
			name = st.sval;
		    }
		    else if (token.equals("type")){
			c = st.nextToken();
			if ((c != StreamTokenizer.TT_WORD) && (c != quoteChar))
			    break outer;
			type = SDDSUtil.identifyType(st.sval);
		    }
		    else if (token.equals("symbol")) {
			c = st.nextToken();
			if ((c != StreamTokenizer.TT_WORD) && (c != quoteChar))
			    break outer;
			symbol = st.sval;
		    } else if (token.equals("units")) {
			c = st.nextToken();
			if ((c != StreamTokenizer.TT_WORD) && (c != quoteChar))
			    break outer;
			units = st.sval;
		    } else if (token.equals("description")) {
			c = st.nextToken();
			if ((c != StreamTokenizer.TT_WORD) && (c != quoteChar))
			    break outer;
			description = st.sval;
		    } else if (token.equals("format_string")) {
			c = st.nextToken();
			if ((c != StreamTokenizer.TT_WORD) && (c != quoteChar))
			    break outer;
			format_string = st.sval;
		    } else if (token.equals("group_name")) {
			c = st.nextToken();
			if ((c != StreamTokenizer.TT_WORD) && (c != quoteChar))
			    break outer;
			group_name = st.sval;
		    } else if (token.equals("field_length")) {
			c = st.nextToken();
			if ((c != StreamTokenizer.TT_WORD) && (c != quoteChar))
			    break outer;
			try {
			    field_length = (Integer.valueOf(st.sval)).intValue();
			} catch (NumberFormatException e) {
			    setError("invalid field_length (parseHeaders)");
			    return false;
			}
		    } else if (token.equals("dimensions")) {
			c = st.nextToken();
			if ((c != StreamTokenizer.TT_WORD) && (c != quoteChar))
			    break outer;
			try {
			    dimensions = (Integer.valueOf(st.sval)).intValue();
			} catch (NumberFormatException e) {
			    setError("invalid dimensions (parseHeaders)");
			    return false;
			}
		    }
		    break;
		case 6:
		    // not supported at this time
		    if (token.equals("&end")){
			header = 0;
			break;
		    }
		    if (token.equals("name")){
			c = st.nextToken();
			if ((c != StreamTokenizer.TT_WORD) && (c != quoteChar))
			    break outer;
			associateName = st.sval;
		    } else if (token.equals("filename")){
			c = st.nextToken();
			if ((c != StreamTokenizer.TT_WORD) && (c != quoteChar))
			    break outer;
			associateFilename = st.sval;
		    }  else if (token.equals("path")){
			c = st.nextToken();
			if ((c != StreamTokenizer.TT_WORD) && (c != quoteChar))
			    break outer;
			associatePath = st.sval;
		    }   else if (token.equals("sdds")){
			c = st.nextToken();
			if ((c != StreamTokenizer.TT_WORD) && (c != quoteChar))
			    break outer;
			associateSdds = st.sval;
		    }
		    break;
		}
	    }
	} catch (Exception e) {
            setError("problem reading header lines (parseHeaders)");
            return false;
        }
	setError("problem reading header lines (parseHeaders)");
	return false;
    }

    /**
     * Read SDDS input file binary data
     *
     * @return            <code>true</code> if successful<br>
     *                    <code>false</code> if an error occurs
     */
    public boolean readBinaryData() {
	int par, col, arr, row, page, rows, i, j, size, arraySize, dimCount = 0;
	int[] dim;
	double dval;
	double[][] doubleValues;
	String[][] stringValues;
	double[][] arrDoubleValues;
	String[][] arrStringValues;
	int[] colDataTypes;
	int[] parDataTypes;
	int[] arrDataTypes;
	byte[] buf;
	char[] c = new char[1];
	doubleValues = new double[columnCount][];
	stringValues = new String[columnCount][];
	arrDoubleValues = new double[arrayCount][];
	arrStringValues = new String[arrayCount][];
	colDataTypes = new int[columnCount];
	parDataTypes = new int[parameterCount];
	arrDataTypes = new int[arrayCount];
	for (i=0;i<columnCount;i++) {
	    colDataTypes[i] = columnObject[i].getType();
	}
	for (i=0;i<parameterCount;i++) {
	    parDataTypes[i] = parameterObject[i].getType();
	}
	for (i=0;i<arrayCount;i++) {
	    arrDataTypes[i] = arrayObject[i].getType();
	}
	page = 1;

	if (bigEndian) {
	    try {
		while (true) {
		  try {
		    rows = dis.readInt();
		  } catch (Exception e) {
		    if (page > 1)
		      return true;
		    setError("Unexpected end of file (readBinaryData)");
		    return false;
		  }
		    for (i=0;i<parameterCount;i++) {
			if (parameterObject[i].fixed)
			    continue;
			switch (parDataTypes[i]) {
			case SDDS_SHORT:
			    setParameter(i,dis.readShort(),page);
			    break;
			case SDDS_LONG:
			    setParameter(i,dis.readInt(),page);
			    break;
			case SDDS_FLOAT:
			    setParameter(i,dis.readFloat(),page);
			    break;
			case SDDS_DOUBLE:
			    setParameter(i,dis.readDouble(),page);
			    break;
			case SDDS_STRING:
			    size = dis.readInt();
			    buf = new byte[size];
			    dis.readFully(buf, 0, size);
			    setParameter(i,(new String(buf)),page);
			    break;
			case SDDS_CHARACTER:
			    setParameter(i,(char)dis.readByte(),page);
			    break;
			}
		    }
		    
 		    for (i=0;i<arrayCount;i++) {
			// read the array size and then read the arrays
			dim = new int[getArrayDimensions(i)];
			while (getArrayDimensions(i) != dimCount) {
			    dim[dimCount] = dis.readInt();
			    dimCount++;
			}
			arraySize = dim[0];
			for (j=1; j<dimCount; j++) {
			    arraySize *= dim[j];
			}
			setArrayDim(page, i, dim);
			dimCount = 0;
			switch (arrDataTypes[i]){
			case SDDS_SHORT:
			case SDDS_LONG:
			case SDDS_FLOAT:
			case SDDS_DOUBLE:
			    arrDoubleValues[i] = new double[arraySize];
			    break;
			case SDDS_STRING:
			case SDDS_CHARACTER:
			    arrStringValues[i] = new String[arraySize];
			    break;
			}
			for (j=0;j<arraySize;j++){
			    switch (arrDataTypes[i]){
			    case SDDS_SHORT:
				arrDoubleValues[i][j] = (double)dis.readShort();
				break;
			    case SDDS_LONG:
				arrDoubleValues[i][j] = (double)dis.readInt();
				break;
			    case SDDS_FLOAT:
				arrDoubleValues[i][j] = (double)dis.readFloat();
				break;
			    case SDDS_DOUBLE:
				arrDoubleValues[i][j] = dis.readDouble();
				break;
			    case SDDS_STRING:
				size = (int)dis.readInt();
				buf = new byte[size];
				dis.readFully(buf, 0, size);
				arrStringValues[i][j] = new String(buf);
				break;
			    case SDDS_CHARACTER:
				c[0] = (char)dis.readByte();
				arrStringValues[i][j] = new String(c);
				break;
			    }
			}
			switch (arrDataTypes[i]){
			case SDDS_SHORT:
			case SDDS_LONG:
			case SDDS_FLOAT:
			case SDDS_DOUBLE:
			    if (!setArray(i,arrDoubleValues[i], page)){
				setError("unable to set array data (readBinaryData)");
				return false;
			    }
			    break;
			case SDDS_STRING:
			case SDDS_CHARACTER:
			    if (!setArray(i,arrStringValues[i], page)){
				setError("unable to set array data (readBinaryData)");
				return false;
			    }
			    break;
			}
		    }
		    if (rows > 0) {
			for (i=0;i<columnCount;i++) {
			    switch (colDataTypes[i]) {
			    case SDDS_SHORT:
			    case SDDS_LONG:
			    case SDDS_FLOAT:
			    case SDDS_DOUBLE:
				doubleValues[i] = new double[rows];
				break;
			    case SDDS_STRING:
			    case SDDS_CHARACTER:
				stringValues[i] = new String[rows];
				break;
			    }
			}
		    }
		    for (row=0;row<rows;row++) {
			for (i=0;i<columnCount;i++) {
			    switch (colDataTypes[i]) {
			    case SDDS_SHORT:
				doubleValues[i][row] = (double)dis.readShort();
				break;
			    case SDDS_LONG:
				doubleValues[i][row] = (double)dis.readInt();
				break;
			    case SDDS_FLOAT:
				doubleValues[i][row] = (double)dis.readFloat();
				break;
			    case SDDS_DOUBLE:
				doubleValues[i][row] = (double)dis.readDouble();
				break;
			    case SDDS_STRING:
				size = (int)dis.readInt();
				buf = new byte[size];
				dis.readFully(buf, 0, size);
				stringValues[i][row] = new String(buf);
				break;
			    case SDDS_CHARACTER:
				c[0] = (char)dis.readByte();
				stringValues[i][row] = new String(c);
				break;
			    }
			}
		    }
		    if (rows > 0) {
			for (i=0;i<columnCount;i++) {
			    switch (colDataTypes[i]) {
			    case SDDS_SHORT:
			    case SDDS_LONG:
			    case SDDS_FLOAT:
			    case SDDS_DOUBLE:
				if (!setColumn(i,doubleValues[i],page)) {
				    setError("unable to set column data (readBinaryData)");
				    return false;
				}
				break;
			    case SDDS_STRING:
			    case SDDS_CHARACTER:
				if (!setColumn(i,stringValues[i],page)) {
				    setError("unable to set column data (readBinaryData)");
				    return false;
				}
				break;
			    }
			}
		    }
		    if (dis.available() == 0)
			return true;
		    page++;
		}
	    } catch (IOException e) {
		setError("error reading binary data (readBinaryData)");
	    }
	} else {
	    try {
	      while (true) {
		try {
		  rows = ledis.readInt();
		} catch (Exception e) {
		  if (page > 1)
		    return true;
		  setError("Unexpected end of file (readBinaryData)");
		  return false;
		}
		    for (i=0;i<parameterCount;i++) {
			if (parameterObject[i].fixed)
			    continue;
			switch (parDataTypes[i]) {
			case SDDS_SHORT:
			    setParameter(i,ledis.readShort(),page);
			    break;
			case SDDS_LONG:
			    setParameter(i,ledis.readInt(),page);
			    break;
			case SDDS_FLOAT:
			    setParameter(i,ledis.readFloat(),page);
			    break;
			case SDDS_DOUBLE:
			    setParameter(i,ledis.readDouble(),page);
			    break;
			case SDDS_STRING:
			    size = ledis.readInt();
			    buf = new byte[size];
			    ledis.readFully(buf, 0, size);
			    setParameter(i,(new String(buf)),page);
			    break;
			case SDDS_CHARACTER:
			    setParameter(i,(char)ledis.readByte(),page);
			    break;
			}
		    }
		    for (i=0;i<arrayCount;i++) {
			// read the array size and then read the arrays
			dim = new int[getArrayDimensions(i)];
			while (getArrayDimensions(i) != dimCount) {
			    dim[dimCount] = ledis.readInt();
			    dimCount++;
			}
			arraySize = dim[0];
			for (j=1; j<dimCount; j++) {
			    arraySize *= dim[j];
			}
			setArrayDim(page, i, dim);
			dimCount = 0;
			switch (arrDataTypes[i]){
			case SDDS_SHORT:
			case SDDS_LONG:
			case SDDS_FLOAT:
			case SDDS_DOUBLE:
			    arrDoubleValues[i] = new double[arraySize];
			    break;
			case SDDS_STRING:
			case SDDS_CHARACTER:
			    arrStringValues[i] = new String[arraySize];
			    break;
			}
			for (j=0;j<arraySize;j++){
			    switch (arrDataTypes[i]){
			    case SDDS_SHORT:
				arrDoubleValues[i][j] = (double)ledis.readShort();
				break;
			    case SDDS_LONG:
				arrDoubleValues[i][j] = (double)ledis.readInt();
				break;
			    case SDDS_FLOAT:
				arrDoubleValues[i][j] = (double)ledis.readFloat();
				break;
			    case SDDS_DOUBLE:
				arrDoubleValues[i][j] = ledis.readDouble();
				break;
			    case SDDS_STRING:
				size = (int)ledis.readInt();
				buf = new byte[size];
				dis.readFully(buf, 0, size);
				arrStringValues[i][j] = new String(buf);
				break;
			    case SDDS_CHARACTER:
				c[0] = (char)ledis.readByte();
				arrStringValues[i][j] = new String(c);
				break;
			    }
			}
			switch (arrDataTypes[i]){
			case SDDS_SHORT:
			case SDDS_LONG:
			case SDDS_FLOAT:
			case SDDS_DOUBLE:
			    if (!setArray(i,arrDoubleValues[i], page)){
				setError("unable to set array data (readBinaryData)");
				return false;
			    }
			    break;
			case SDDS_STRING:
			case SDDS_CHARACTER:
			    if (!setArray(i,arrStringValues[i], page)){
				setError("unable to set array data (readBinaryData)");
				return false;
			    }
			    break;
			}
		    }		    
		    if (rows > 0) {
			for (i=0;i<columnCount;i++) {
			    switch (colDataTypes[i]) {
			    case SDDS_SHORT:
			    case SDDS_LONG:
			    case SDDS_FLOAT:
			    case SDDS_DOUBLE:
				doubleValues[i] = new double[rows];
				break;
			    case SDDS_STRING:
			    case SDDS_CHARACTER:
				stringValues[i] = new String[rows];
				break;
			    }
			}
		    }
		    for (row=0;row<rows;row++) {
			for (i=0;i<columnCount;i++) {
			    switch (colDataTypes[i]) {
			    case SDDS_SHORT:
				doubleValues[i][row] = (double)ledis.readShort();
				break;
			    case SDDS_LONG:
				doubleValues[i][row] = (double)ledis.readInt();
				break;
			    case SDDS_FLOAT:
				doubleValues[i][row] = (double)ledis.readFloat();
				break;
			    case SDDS_DOUBLE:
				doubleValues[i][row] = (double)ledis.readDouble();
				break;
			    case SDDS_STRING:
				size = (int)ledis.readInt();
				buf = new byte[size];
				ledis.readFully(buf, 0, size);
				stringValues[i][row] = new String(buf);
				break;
			    case SDDS_CHARACTER:
				c[0] = (char)ledis.readByte();
				stringValues[i][row] = new String(c);
				break;
			    }
			}
		    }
		    if (rows > 0) {
			for (i=0;i<columnCount;i++) {
			    switch (colDataTypes[i]) {
			    case SDDS_SHORT:
			    case SDDS_LONG:
			    case SDDS_FLOAT:
			    case SDDS_DOUBLE:
				if (!setColumn(i,doubleValues[i],page)) {
				    setError("unable to set column data (readBinaryData)");
				    return false;
				}
				break;
			    case SDDS_STRING:
			    case SDDS_CHARACTER:
				if (!setColumn(i,stringValues[i],page)) {
				    setError("unable to set column data (readBinaryData)");
				    return false;
				}
				break;
			    }
			}
		    }
		    if (dis.available() == 0)
			return true;
		    page++;
		}
	    } catch (IOException e) {
		setError("error reading binary data (readBinaryData)");
	    }
	}
	setError("error reading binary data (readBinaryData)");
	return false;
    }

    /**
     * Read SDDS input file ascii data
     *
     * @return            <code>true</code> if successful<br>
     *                    <code>false</code> if an error occurs
     */
    public boolean readAsciiData() {
        StreamTokenizer st;
	int par, arr, col, row, page, rows, i, arraySize=0, c=0, dimCount=0;
	int[] dim;
	double[][] doubleValues;
	String[][] stringValues;
	double[] doubleArrValues;
	String[] stringArrValues;
	int[] datatypes;
	int[] parDataTypes;
	int[] arrDataTypes;
	boolean endOfPage = true;
	boolean readyToReadArray = false;
	boolean moreRows = true;
	final int quoteChar = 34;
	boolean skipRead = false;
	doubleValues = new double[columnCount][];
	stringValues = new String[columnCount][];
	datatypes = new int[columnCount];
	parDataTypes = new int[parameterCount];
	arrDataTypes = new int[arrayCount];
	stringArrValues = new String[arrayCount];
	doubleArrValues = new double[arrayCount];
	for (i=0;i<columnCount;i++) {
	    datatypes[i] = columnObject[i].getType();
	}
	for (i=0;i<parameterCount;i++) {
	    parDataTypes[i] = parameterObject[i].getType();
	}
	for (i=0;i<arrayCount;i++){
	    arrDataTypes[i] = arrayObject[i].getType();
	}
	
	par = col = row = arr = 0;
	page = 1;
	rows = -1;

	try {
	    st = new StreamTokenizer(dis);
	    st.resetSyntax();
	    st.whitespaceChars('\u0000','\u0020');
	    st.commentChar('!');
	    st.quoteChar(34);
	    st.wordChars(35,255);
	    st.eolIsSignificant(true);
	    st.lowerCaseMode(false);
	    while (true) {
		if (!skipRead) {
		    c = st.nextToken();
		    if (c == StreamTokenizer.TT_EOF) {
			if (endOfPage){
			    return true;
			}
			setError("unexpected end of file (readAsciiData)");
			break;
		    } else if (c == StreamTokenizer.TT_EOL) {
			continue;
		    }
		} else {
		    skipRead = false;
		}
		endOfPage = false;
		if (par < parameterCount) {
		    if (parameterObject[par].fixed) {
			skipRead = true;
		    } else {
			if ((c != StreamTokenizer.TT_WORD) && (c != quoteChar)) {
			    setError("invalid parameter value at line #" + st.lineno() + " (readAsciiData)");
			    return false;
			}
			switch (parDataTypes[par]) {
			case SDDS_STRING:
			    if (!setParameter(par, SDDSUtil.removeBackSlashes(st.sval), page)) {
				setError("unable to set parameter at line #" + st.lineno() + " (readAsciiData)");
				return false;
			    }
			    break;
			case SDDS_CHARACTER:
			    if (!setParameter(par, SDDSUtil.returnCharValueFromOctalCode(st.sval), page)) {
				setError("unable to set parameter at line #" + st.lineno() + " (readAsciiData)");
				return false;
			    }
			    break;
			default:		       
			    if (!setParameter(par, st.sval, page)) {
				setError("unable to set parameter at line #" + st.lineno() + " (readAsciiData)");
				return false;
			    }
			    break;
			}
		    }
		    par++;
		} else if (arr < arrayCount && !readyToReadArray) {
		    arraySize = 0;
		    dim = new int[getArrayDimensions(arr)];
		    while (getArrayDimensions(arr) != dimCount) {
			if ((c != StreamTokenizer.TT_WORD) && (c != quoteChar)) {
			    setError("invalid array data at line #" + st.lineno() + " (readAsciiData)");
			    return false;
			}
			try {
			    dim[dimCount] = Integer.valueOf(st.sval).intValue();
			}
			catch (Exception e) {
			    setError("invalid array size at line #" + st.lineno() + " (readAsciiData)");
			    return false;
			}
			dimCount++;
			if (getArrayDimensions(arr) != dimCount)
			    c = st.nextToken();
		    }
		    arraySize = dim[0];
		    for (i=1; i<dimCount; i++) {
			arraySize *= dim[i];
		    }
		    setArrayDim(page, arr, dim);
		    dimCount = 0;
		    if (arraySize > 0){
			switch (arrDataTypes[arr]){
			case SDDS_SHORT:
			case SDDS_LONG:
			case SDDS_FLOAT:
			case SDDS_DOUBLE:
			    doubleArrValues = new double[arraySize];
			    break;
			case SDDS_STRING:
			case SDDS_CHARACTER:
			    stringArrValues = new String[arraySize];
			    break;
			}
		    }
		    readyToReadArray = true;
		} else if (readyToReadArray){
		    readyToReadArray = false;
		    for (i=0;i<arraySize;i++){
			if (( c != StreamTokenizer.TT_WORD) && (c != quoteChar)){
			    setError("invalid array element at line #" + st.lineno() + " (readAsciiData)");
			    return false;
			}
			switch (arrDataTypes[arr]){
			case SDDS_STRING:
			    stringArrValues[i] = SDDSUtil.removeBackSlashes(st.sval);
			    break;
			case SDDS_CHARACTER:
			    stringArrValues[i] = String.valueOf(SDDSUtil.returnCharValueFromOctalCode(st.sval));
			    break;
			default:
			    doubleArrValues[i] = (Double.valueOf(st.sval)).doubleValue();
			    break;
			}
			if (i<arraySize-1)
			    c = st.nextToken();
			while (c == StreamTokenizer.TT_EOL)
			    c = st.nextToken();
			if (c == StreamTokenizer.TT_EOF) {
			    setError("unexpected end of file (readAsciiData)");
			    return false;
			}
		    }
		    switch (arrDataTypes[arr]){
		    case SDDS_STRING:
		    case SDDS_CHARACTER:
			if (!setArray(arr, stringArrValues, page)){
			    setError("unable to set array (readAsciiData)");
			    return false;
			}
			break;
		    case SDDS_SHORT:
		    case SDDS_LONG:
		    case SDDS_FLOAT:
		    case SDDS_DOUBLE:
			if (!setArray(arr, doubleArrValues, page)){
			    setError("unable to set array (readAsciiData)");
			    return false;
			}
			break;
		    }
		    arr++;
		} else if ((rows == -1) && (columnCount > 0)) {
		    if (!noRowCount) {
			if ((c != StreamTokenizer.TT_WORD) && (c != quoteChar)) {
			    setError("invalid row value at line #" + st.lineno() + " (readAsciiData)");
			    return false;
			}
			try {
			    rows = Integer.valueOf(st.sval).intValue();
			} catch (Exception e) {
			    setError("invalid row value at line #" + st.lineno() + " (readAsciiData)");
			    return false;
			}
		    }
		    if ((rows > 0) || noRowCount) {
			if (noRowCount) {
			    rows = 1;
			    skipRead = true;
			}
			for (i=0;i<columnCount;i++) {
			    switch (datatypes[i]) {
			    case SDDS_SHORT:
			    case SDDS_LONG:
			    case SDDS_FLOAT:
			    case SDDS_DOUBLE:
				doubleValues[i] = new double[rows];
				break;
			    case SDDS_STRING:
			    case SDDS_CHARACTER:
				stringValues[i] = new String[rows];
				break;
			    }
			}
		    }
		} else if (columnCount > 0) {
		    while ((row < rows) || (noRowCount && moreRows)) {
			while (col < columnCount) {
			    if (noRowCount && (c == StreamTokenizer.TT_EOF)) {
				moreRows = false;
				rows--;
				break;
			    } else if (noRowCount && (c == StreamTokenizer.TT_EOL)) {
				moreRows = false;
				rows--;
				skipRead = true;
				c = st.nextToken();
				break;
			    } else if ((c != StreamTokenizer.TT_WORD) && (c != quoteChar)) {
				setError("invalid column element at line #" + st.lineno() + " (readAsciiData)");
				return false;
			    }
			    switch (datatypes[col]) {
			    case SDDS_STRING:
				if (noRowCount && row >= Array.getLength(stringValues[col]))
				    stringValues[col] = (String[])SDDSUtil.resize(stringValues[col], row+1);
				stringValues[col][row] = SDDSUtil.removeBackSlashes(st.sval);
				break;
			    case SDDS_CHARACTER:
				if (noRowCount && row >= Array.getLength(stringValues[col]))
				    stringValues[col] = (String[])SDDSUtil.resize(stringValues[col], row+1);
				stringValues[col][row] = String.valueOf(SDDSUtil.returnCharValueFromOctalCode(st.sval));
				break;
			    default:
				if (noRowCount && row >= Array.getLength(doubleValues[col]))
				    doubleValues[col] = (double[])SDDSUtil.resize(doubleValues[col], row+1);
				doubleValues[col][row] = (Double.valueOf(st.sval)).doubleValue();
				break;
			    }
			    col++;
			    if (col == columnCount) {
				if (!noRowCount)
				    row++;
				c = st.nextToken();
				while (c != StreamTokenizer.TT_EOL) {
				    if (c == StreamTokenizer.TT_EOF) {
					if ((row == rows) || noRowCount)
					    break;
					setError("unexpected end of file (readAsciiData)");
					return false;
				    }
				    c = st.nextToken();
				}
				if (row < rows) {
				    col = 0;
				    c = st.nextToken();
				}
				break;
			    }
			    c = st.nextToken();
			    while (c == StreamTokenizer.TT_EOL)
				c = st.nextToken();
			    if (c == StreamTokenizer.TT_EOF) {
				setError("unexpected end of file (readAsciiData)");
				return false;
			    }
			}
			if (noRowCount && moreRows){
			    row++;
			    rows++;
			}
		    } 
		}
		if ((par == parameterCount) && 
		    ((col == columnCount) || (rows == 0)) && 
		    ((columnCount == 0) || (row == rows)) &&
		    (arr == arrayCount) || (noRowCount && !moreRows)) {
		    par = col = row = arr = 0;
		    endOfPage = true;
		    moreRows = true;
		}
		if (endOfPage) {
		    if (rows > 0) {
			for (i=0;i<columnCount;i++) {
			    switch (datatypes[i]) {
			    case SDDS_SHORT:
			    case SDDS_LONG:
			    case SDDS_FLOAT:
			    case SDDS_DOUBLE:
				if (!setColumn(i,doubleValues[i],page)) {
				    setError("unable to set column (readAsciiData)");
				    return false;
				}
				break;
			    case SDDS_STRING:
			    case SDDS_CHARACTER:
				if (!setColumn(i,stringValues[i],page)) {
				    setError("unable to set column (readAsciiData)");
				    return false;
				}
				break;
			    }
			}
		    } else {
			makeBlankPages(page);
		    }
		    rows = -1;
		    page++;
		}
		if (!skipRead) {
		    c = st.nextToken();
		    while (c != StreamTokenizer.TT_EOL) {
			if (c == StreamTokenizer.TT_EOF) {
			    if (endOfPage){
				return true;
			    }
			    setError("unexpected end of file (readAsciiData)");
			    return false;
			}
			c = st.nextToken();
		    }
		}
	    }
	} catch(Exception e) {
            setError("problem reading ascii data (readAsciiData)");
            return false;
        }
	setError("problem reading ascii data (readAsciiData)");
	return false;
    }

    /**
     * Create blank pages up to the last page specified
     * 
     * @param lastpage         last page to create blank page
     */
    public void makeBlankPages(int lastpage) {
	int i;
	for (i=0;i<columnCount;i++) {
	    columnObject[i].makeBlankPages(lastpage);
	}
    }

    /**
     * Insert blank page at index specifed
     * 
     * @param page             index of page to create;
     */
    public boolean insertPage(int page) {
	int i;
	for (i=0;i<parameterCount;i++) {
	    if (!parameterObject[i].insertPage(page)) {
		setError("problem encountered while inserting page (insertPage)");
		return false;
	    }
	}
	for (i=0;i<arrayCount;i++){
	    if (!arrayObject[i].insertPage(page)){
		setError("problem encountered while inserting page (insertPage)");
		return false;
	    }
	}
	for (i=0;i<columnCount;i++) {
	    if (!columnObject[i].insertPage(page)) {
		setError("problem encountered while inserting page (insertPage)");
		return false;
	    }
	}
	return true;
    }

    /**
     * Delete page at index specifed
     * 
     * @param page             index of page to delete;
     */
    public boolean deletePage(int page) {
	int i;
	for (i=0;i<parameterCount;i++) {
	    if (!parameterObject[i].deletePage(page)) {
		setError("problem encountered while deleteing page (deletePage)");
		return false;
	    }
	}
	for (i=0;i<arrayCount;i++){
	    if (!arrayObject[i].deletePage(page)){
		setError("problem encountered while deleting page (deletePage)");
		return false;
	    }
	}
	for (i=0;i<columnCount;i++) {
	    if (!columnObject[i].deletePage(page)) {
		setError("problem encountered while deleteing page (deletePage)");
		return false;
	    }
	}
	return true;
    }


    /**
     * Close the SDDS output file
     * 
     * @return              <code>true</code> if successful<br>
     *                      <code>false</code> if error occurs
     */
    public boolean closeOutput() {
      try {
	dos.close();
      } catch(Exception e) {
	setError("problem closing output file (closeOutput)");
	return false;
      }
      
      return true;
    }

    /**
     * Close the SDDS input file
     * 
     * @return              <code>true</code> if successful<br>
     *                      <code>false</code> if error occurs
     */
    public boolean closeInput() {
        try {
            dis.close();
        } catch(IOException e) {
            setError("problem closing input file (closeInput)");
            return false;
        }
	return true;
    }

    /**
     * Set the description text and contents.
     * 
     * @param text          may be <code>null</code>
     * @param contents      may be <code>null</code>
     */
    public void setDescription(String text, String contents) {
	descriptionText = text;
	descriptionContents = contents;
	return; 
    }

    /**
     * Delete parameter
     * 
     * @param name          name of the parameter
     */
    public void deleteParameter(String name) {
	int i, j;
	i = getParameterIndex(name);
	if (i == -1)
	    return;
	if (parameterCount == 1) {
	    parameterObject[0] = null;
	} else {
	    for (j=i;j<parameterCount-1;j++) {
		parameterObject[j] = parameterObject[j+1];
	    }
	    parameterObject[parameterCount-1] = null;
	}
	parameterCount--;
    }

    /**
     * Delete array
     *
     * @param name          name of the array
     */
    public void deleteArray(String name)
    {
	int i, j;
	i = getArrayIndex(name);
	if (i == -1)
	    return;
	if (arrayCount == 1){
	    arrayObject[0] = null;
	}
	else {
	    for (j=i;j<arrayCount-1;j++){
		arrayObject[j] = arrayObject[j+1];
	    }
	    arrayObject[arrayCount-1] = null;
	}
	arrayCount--;
    }
    
    /**
     * Delete column
     * 
     * @param name          name of the column
     */
    public void deleteColumn(String name) {
	int i, j;
	i = getColumnIndex(name);
	if (i == -1)
	    return;
	if (columnCount == 1) {
	    columnObject[0] = null;
	} else {
	    for (j=i;j<columnCount-1;j++) {
		columnObject[j] = columnObject[j+1];
	    }
	    columnObject[columnCount-1] = null;
	}
	columnCount--;
    }

    /**
     * Rename parameter
     * 
     * @param oldname          old name of the parameter
     * @param newname          new name of the parameter
     */
    public void renameParameter(String oldname, String newname) {
	int i, j;
	i = getParameterIndex(oldname);
	if (i == -1)
	    return;
	parameterObject[i].setName(newname);
    }

    /**
     * Rename array
     *
     * @param oldname          old name of the array
     * @param newname          new name of the array
     */
    public void renameArray(String oldname, String newname)
    {
	int i, j;
	i = getArrayIndex(oldname);
	if (i==-1)
	    return;
	arrayObject[i].setName(newname);
    }

    /**
     * Rename column
     * 
     * @param oldname          old name of the column
     * @param newname          new name of the column
     */
    public void renameColumn(String oldname, String newname) {
	int i, j;
	i = getColumnIndex(oldname);
	if (i == -1)
	    return;
	columnObject[i].setName(newname);
    }

    /**
     * Set column symbol
     * 
     * @param index            index of the column
     * @param symbol           symbol for the column
     */
    public void setColumnSymbol(int index, String symbol) {
	if ((index < 0) || (index >= columnCount))
	    return;
	columnObject[index].setSymbol(symbol);
    }

    /**
     * Set array symbol
     * 
     * @param index            index of the array
     * @param symbol           symbol for the array
     */
    public void setArraySymbol(int index, String symbol) {
	if ((index < 0) || (index >= arrayCount))
	    return;
	arrayObject[index].setSymbol(symbol);
    }
    
    /**
     * Set parameter symbol
     * 
     * @param index            index of the parameter
     * @param symbol           symbol for the parameter
     */
    public void setParameterSymbol(int index, String symbol) {
	if ((index < 0) || (index >= parameterCount))
	    return;
	parameterObject[index].setSymbol(symbol);
    }

    /**
     * Set column units
     * 
     * @param index            index of the column
     * @param units            units for the column
     */
    public void setColumnUnits(int index, String units) {
	if ((index < 0) || (index >= columnCount))
	    return;
	columnObject[index].setUnits(units);
    }

    /**
     * Set array units
     * 
     * @param index            index of the array
     * @param units            units for the array
     */
    public void setArrayUnits(int index, String units) {
	if ((index < 0) || (index >= arrayCount))
	    return;
	arrayObject[index].setUnits(units);
    }
    
    /**
     * Set parameter units
     * 
     * @param index            index of the parameter
     * @param units            units for the parameter
     */
    public void setParameterUnits(int index, String units) {
	if ((index < 0) || (index >= parameterCount))
	    return;
	parameterObject[index].setUnits(units);
    }

    /**
     * Set column description
     * 
     * @param index            index of the column
     * @param description      description for the column
     */
    public void setColumnDescription(int index, String description) {
	if ((index < 0) || (index >= columnCount))
	    return;
	columnObject[index].setDescription(description);
    }

    /**
     * Set array description
     * 
     * @param index            index of the array
     * @param description      description for the array
     */
    public void setArrayDescription(int index, String description) {
	if ((index < 0) || (index >= arrayCount))
	    return;
	arrayObject[index].setDescription(description);
    }

    /**
     * Set parameter description
     * 
     * @param index            index of the parameter
     * @param description      description for the parameter
     */
    public void setParameterDescription(int index, String description) {
	if ((index < 0) || (index >= parameterCount))
	    return;
	parameterObject[index].setDescription(description);
    }

    /**
     * Set column format string
     * 
     * @param index            index of the column
     * @param format_string    format string for the column
     */
    public void setColumnFormatString(int index, String format_string) {
	if ((index < 0) || (index >= columnCount))
	    return;
	columnObject[index].setFormat_String(format_string);
    }

    /**
     * Set array format string
     * 
     * @param index            index of the array
     * @param format_string    format string for the array
     */
    public void setArrayFormatString(int index, String format_string) {
	if ((index < 0) || (index >= arrayCount))
	    return;
	arrayObject[index].setFormat_String(format_string);
    }

    /**
     * Set parameter format string
     * 
     * @param index            index of the parameter
     * @param format_string    format string for the parameter
     */
    public void setParameterFormatString(int index, String format_string) {
	if ((index < 0) || (index >= parameterCount))
	    return;
	parameterObject[index].setFormat_String(format_string);
    }

    /**
     * Set array group name
     * 
     * @param index            index of the array
     * @param group_name    group name for the array
     */
    public void setArrayGroupName(int index, String group_name) {
	if ((index < 0) || (index >= arrayCount))
	    return;
	arrayObject[index].setGroup_Name(group_name);
    }

    /**
     * Set column field length
     * 
     * @param index            index of the column
     * @param field_length     field length for the column
     */
    public void setColumnFieldLength(int index, int field_length) {
	if ((index < 0) || (index >= columnCount))
	    return;
	columnObject[index].setField_Length(field_length);
    }

    /**
     * Set array field length
     * 
     * @param index            index of the array
     * @param field_length     field length for the array
     */
    public void setArrayFieldLength(int index, int field_length) {
	if ((index < 0) || (index >= arrayCount))
	    return;
	arrayObject[index].setField_Length(field_length);
    }

    /**
     * Set parameter fixed value
     * 
     * @param index            index of the parameter
     * @param fixed_value      fixed_value for the parameter
     */
    public void setParameterFixedValue(int index, String fixed_value) {
	if ((index < 0) || (index >= parameterCount))
	    return;
	parameterObject[index].setFixed_Value(fixed_value);
    }

    /**
     * Get parameter names
     * 
     * @return            string array of parameter names
     */
    public String[] getParameterNames() {
	int i;
	String[] names;
	names = new String[parameterCount];
	for (i=0;i<parameterCount;i++) {
	    names[i] = getParameterName(i);
	}
	return names;
    }

    /**
     * Get parameter name
     * 
     * @param index       index of the parameter
     * @return            parameter name
     */
    public String getParameterName(int index) {
	if ((index < 0) || (index >= parameterCount))
	    return null;
	return parameterObject[index].getName();
    }

    /**
     * Get array names
     *
     * @return            string of array names
     */
    public String[] getArrayNames() 
    {
	int i;
	String[] names;
	names = new String[arrayCount];
	for (i=0;i<arrayCount;i++){
	    names[i] = getArrayName(i);
	}
	return names;
    }

    /**
     * Get array name
     *
     * @param index       index of the array
     * @return            array name
     */
    public String getArrayName(int index) 
    {
	if ((index < 0) || (index >= arrayCount))
	    return null;
	return arrayObject[index].getName();
    }
    
    /**
     * Get column names
     * 
     * @return            string array of column names
     */
    public String[] getColumnNames() {
	int i;
	String[] names;
	names = new String[columnCount];
	for (i=0;i<columnCount;i++) {
	    names[i] = getColumnName(i);
	}
	return names;
    }

    /**
     * Get column name
     * 
     * @param index       index of the column
     * @return            column name
     */
    public String getColumnName(int index) {
	if ((index < 0) || (index >= columnCount))
	    return null;
	return columnObject[index].getName();
    }

    /**
     * Get all parameter units
     * 
     * @return            string array of parameter units
     */
    public String[] getAllParameterUnits() {
	int i;
	String[] units;
	units = new String[parameterCount];
	for (i=0;i<parameterCount;i++) {
	    units[i] = getParameterUnits(i);
	}
	return units;
    }

    /**
     * Get parameter units
     * 
     * @param index       index of the parameter
     * @return            parameter units
     */
    public String getParameterUnits(int index) {
	if ((index < 0) || (index >= parameterCount))
	    return null;
	return parameterObject[index].getUnits();
    }

    /**
     * Get all array units
     *
     * @return            string array of array units
     */
    public String[] getAllArrayUnits()
    {
	int i;
	String[] units;
	units = new String[arrayCount];
	for (i=0;i<arrayCount;i++){
	    units[i] = getArrayUnits(i);
	}
	return units;
    }

    /**
     * Get array units
     *
     * @param index       index of the array
     * @return            array units
     */
    public String getArrayUnits(int index)
    {
	if ((index < 0) || (index >= arrayCount))
	    return null;
	return arrayObject[index].getUnits();
    }
    
    /**
     * Get all column units
     * 
     * @return            string array of column units
     */
    public String[] getAllColumnUnits() {
	int i;
	String[] units;
	units = new String[columnCount];
	for (i=0;i<columnCount;i++) {
	    units[i] = getColumnUnits(i);
	}
	return units;
    }

    /**
     * Get column units
     * 
     * @param index       index of the column
     * @return            column units
     */
    public String getColumnUnits(int index) {
	if ((index < 0) || (index >= columnCount))
	    return null;
	return columnObject[index].getUnits();
    }

    /**
     * Get parameter symbols
     * 
     * @return            string array of parameter symbols
     */
    public String[] getParameterSymbols() {
	int i;
	String[] symbols;
	symbols = new String[parameterCount];
	for (i=0;i<parameterCount;i++) {
	    symbols[i] = getParameterSymbol(i);
	}
	return symbols;
    }

    /**
     * Get parameter symbol
     * 
     * @param index       index of the parameter
     * @return            parameter symbol
     */
    public String getParameterSymbol(int index) {
	if ((index < 0) || (index >= parameterCount))
	    return null;
	return parameterObject[index].getSymbol();
    }

    /**
     * Get array symbols
     * 
     * @return            string array of array symbols
     */
    public String[] getArraySymbols() {
	int i;
	String[] symbols;
	symbols = new String[arrayCount];
	for (i=0;i<arrayCount;i++) {
	    symbols[i] = getArraySymbol(i);
	}
	return symbols;
    }

    /**
     * Get array symbol
     * 
     * @param index       index of the array
     * @return            array symbol
     */
    public String getArraySymbol(int index) {
	if ((index < 0) || (index >= arrayCount))
	    return null;
	return arrayObject[index].getSymbol();
    }
    
    /**
     * Get column symbols
     * 
     * @return            string array of column symbols
     */
    public String[] getColumnSymbols() {
	int i;
	String[] symbols;
	symbols = new String[columnCount];
	for (i=0;i<columnCount;i++) {
	    symbols[i] = getColumnSymbol(i);
	}
	return symbols;
    }

    /**
     * Get column symbol
     * 
     * @param index       index of the column
     * @return            column symbol
     */
    public String getColumnSymbol(int index) {
	if ((index < 0) || (index >= columnCount))
	    return null;
	return columnObject[index].getSymbol();
    }

    /**
     * Get parameter format strings
     * 
     * @return            string array of parameter format strings
     */
    public String[] getParameterFormatStrings() {
	int i;
	String[] format_strings;
	format_strings = new String[parameterCount];
	for (i=0;i<parameterCount;i++) {
	    format_strings[i] = getParameterFormatString(i);
	}
	return format_strings;
    }

    /**
     * Get parameter format string
     * 
     * @param index       index of the parameter
     * @return            parameter format string
     */
    public String getParameterFormatString(int index) {
	if ((index < 0) || (index >= parameterCount))
	    return null;
	return parameterObject[index].getFormat_String();
    }

    /**
     * Get array format strings
     * 
     * @return            string array of array format strings
     */
    public String[] getArrayFormatStrings() {
	int i;
	String[] format_strings;
	format_strings = new String[arrayCount];
	for (i=0;i<arrayCount;i++) {
	    format_strings[i] = getArrayFormatString(i);
	}
	return format_strings;
    }

    /**
     * Get array format string
     * 
     * @param index       index of the array
     * @return            array format string
     */
    public String getArrayFormatString(int index) {
	if ((index < 0) || (index >= arrayCount))
	    return null;
	return arrayObject[index].getFormat_String();
    }

    /**
     * Get column format strings
     * 
     * @return            string array of column format strings
     */
    public String[] getColumnFormatStrings() {
	int i;
	String[] format_strings;
	format_strings = new String[columnCount];
	for (i=0;i<columnCount;i++) {
	    format_strings[i] = getColumnFormatString(i);
	}
	return format_strings;
    }

    /**
     * Get column format string
     * 
     * @param index       index of the column
     * @return            column format string
     */
    public String getColumnFormatString(int index) {
	if ((index < 0) || (index >= columnCount))
	    return null;
	return columnObject[index].getFormat_String();
    }

    /**
     * Get array group names
     * 
     * @return            string array of array group names
     */
    public String[] getArrayGroupNames() {
	int i;
	String[] group_names;
	group_names = new String[arrayCount];
	for (i=0;i<arrayCount;i++) {
	    group_names[i] = getArrayGroupName(i);
	}
	return group_names;
    }

    /**
     * Get array group name
     * 
     * @param index       index of the array
     * @return            array group name
     */
    public String getArrayGroupName(int index) {
	if ((index < 0) || (index >= arrayCount))
	    return null;
	return arrayObject[index].getGroup_Name();
    }

    /**
     * Get parameter types
     * 
     * @return            int array of parameter types
     */
    public int[] getParameterTypes() {
	int i;
	int[] types;
	types = new int[parameterCount];
	for (i=0;i<parameterCount;i++) {
	    types[i] = getParameterType(i);
	}
	return types;
    }

    /**
     * Get parameter type
     * 
     * @param index       index of the parameter
     * @return            parameter type
     */
    public int getParameterType(int index) {
	if ((index < 0) || (index >= parameterCount))
	    return 0;
	return parameterObject[index].getType();
    }

    /**
     * Get array types
     * 
     * @return            int array of array types
     */
    public int[] getArrayTypes() {
	int i;
	int[] types;
	types = new int[arrayCount];
	for (i=0;i<arrayCount;i++) {
	    types[i] = getArrayType(i);
	}
	return types;
    }

    /**
     * Get array type
     * 
     * @param index       index of the array
     * @return            array type
     */
    public int getArrayType(int index) {
	if ((index < 0) || (index >= arrayCount))
	    return 0;
	return arrayObject[index].getType();
    }

    /**
     * Get column types
     * 
     * @return            int array of column types
     */
    public int[] getColumnTypes() {
	int i;
	int[] types;
	types = new int[columnCount];
	for (i=0;i<columnCount;i++) {
	    types[i] = getColumnType(i);
	}
	return types;
    }

    /**
     * Get column type
     * 
     * @param index       index of the column
     * @return            column type
     */
    public int getColumnType(int index) {
	if ((index < 0) || (index >= columnCount))
	    return 0;
	return columnObject[index].getType();
    }

    /**
     * Get field lengths
     * 
     * @return            int array of column field lengths
     */
    public int[] getColumnFieldLengths() {
	int i;
	int[] lengths;
	lengths = new int[columnCount];
	for (i=0;i<columnCount;i++) {
	    lengths[i] = getColumnFieldLength(i);
	}
	return lengths;
    }

    /**
     * Get field lengths
     * 
     * @return            int array of array field lengths
     */
    public int[] getArrayFieldLengths() {
	int i;
	int[] lengths;
	lengths = new int[arrayCount];
	for (i=0;i<arrayCount;i++) {
	    lengths[i] = getArrayFieldLength(i);
	}
	return lengths;
    }

    /**
     * Get column field length
     * 
     * @param index       index of the column
     * @return            column field length
     */
    public int getColumnFieldLength(int index) {
	if ((index < 0) || (index >= columnCount))
	    return 0;
	return columnObject[index].getField_Length();
    }

    /**
     * Get array field length
     * 
     * @param index       index of the array
     * @return            array field length
     */
    public int getArrayFieldLength(int index) {
	if ((index < 0) || (index >= arrayCount))
	    return 0;
	return arrayObject[index].getField_Length();
    }

    /**
     * Get array dimensions
     * 
     * @param index       index of the array
     * @return            array dimensions
     */
    public int getArrayDimensions(int index) {
	if ((index < 0) || (index >= arrayCount))
	    return 0;
	return arrayObject[index].getDimensions();
    }

    /**
     * Returns the number of elements in each dimension of the array
     * 
     * @param page        index of the page
     * @param index       index of the array
     * @return            array of sdds array dimensions
     */
    public int[] getArrayDim(int page, int index) {
	if ((index < 0) || (index >= arrayCount))
	    return null;
	return arrayObject[index].getDim(page);
    }

    /**
     * Sets the number of elements in each dimension of the array
     * 
     * @param page        index of the page
     * @param index       index of the array
     * @param dim         array of sdds array dimensions
     */
    public void setArrayDim(int page, int index, int[] dim) {
	arrayObject[index].setDim(page, dim);
    }

    /**
     * Get parameter descriptions
     * 
     * @return            string array of parameter descriptions
     */
    public String[] getParameterDescriptions() {
	int i;
	String[] descriptions;
	descriptions = new String[parameterCount];
	for (i=0;i<parameterCount;i++) {
	    descriptions[i] = getParameterDescription(i);
	}
	return descriptions;
    }

    /**
     * Get parameter description
     * 
     * @param index       index of the parameter
     * @return            parameter description
     */
    public String getParameterDescription(int index) {
	if ((index < 0) || (index >= parameterCount))
	    return null;
	return parameterObject[index].getDescription();
    }

    /**
     * Get array descriptions
     * 
     * @return            string array of array descriptions
     */
    public String[] getArrayDescriptions() {
	int i;
	String[] descriptions;
	descriptions = new String[arrayCount];
	for (i=0;i<arrayCount;i++) {
	    descriptions[i] = getArrayDescription(i);
	}
	return descriptions;
    }

    /**
     * Get array description
     * 
     * @param index       index of the array
     * @return            array description
     */
    public String getArrayDescription(int index) {
	if ((index < 0) || (index >= arrayCount))
	    return null;
	return arrayObject[index].getDescription();
    }

    /**
     * Get column descriptions
     * 
     * @return            string array of column descriptions
     */
    public String[] getColumnDescriptions() {
	int i;
	String[] descriptions;
	descriptions = new String[columnCount];
	for (i=0;i<columnCount;i++) {
	    descriptions[i] = getColumnDescription(i);
	}
	return descriptions;
    }

    /**
     * Get column description
     * 
     * @param index       index of the column
     * @return            column description
     */
    public String getColumnDescription(int index) {
	if ((index < 0) || (index >= columnCount))
	    return null;
	return columnObject[index].getDescription();
    }

    /**
     * Get parameter fixed value
     * 
     * @param index       index of the parameter
     * @return            parameter's fixed value
     */
    public String getParameterFixedValue(int index) {
	if ((index < 0) || (index >= parameterCount))
	    return null;
	return parameterObject[index].getFixed_Value();
    }

    /**
     * Get the values of the column for a chosen page
     *
     * @param index           index of the column
     * @param string_quotes   <code>true</code> if quotes are to be added around strings<br>
     *                        <code>false</code> if raw strings are to be outputed.
     * @param page            the page of the column, must be greater then <code>0</code>
     * @return                Object array of data
     */
    public Object[] getColumnValues(int index, int page, boolean string_quotes) {
	if ((index < 0) || (index >= columnCount))
	    return null;
	return columnObject[index].getValues(page,string_quotes);
    }

    /**
     * Get the values of the array for a chosen page
     *
     * @param index           index of the array
     * @param string_quotes   <code>true</code> if quotes are to be added around strings<br>
     *                        <code>false</code> if raw strings are to be outputed.
     * @param page            the page of the array, must be greater then <code>0</code>
     * @return                Object array of data
     */
    public Object[] getArrayValues(int index, int page, boolean string_quotes) {
	if ((index < 0) || (index >= arrayCount))
	    return null;
	return arrayObject[index].getValues(page,string_quotes);
    }

    /**
     * Get the value of the parameter for a chosen page
     *
     * @param index           index of the parameter
     * @param string_quotes   <code>true</code> if quotes are to be added around strings<br>
     *                        <code>false</code> if raw strings are to be outputed.
     * @param page            the page of the parameter, must be greater then <code>0</code>
     * @return                Object containing parameter data
     */
    public Object getParameterValue(int index, int page, boolean string_quotes) {
	if ((index < 0) || (index >= parameterCount))
	    return null;
	return parameterObject[index].getValue(page,string_quotes);
    }

    
    /**
     * Creates a new parameter with the specified settings.
     * 
     * @param name             should be a valid name as defined by
     *                         SDDSUtil.isValidName
     * @param symbol           may be <code>null</code>
     * @param units            may be <code>null</code>
     * @param description      may be <code>null</code>
     * @param format_string    may be <code>null</code>
     * @param type             valid values are:
     *                         <ul>
     *                         <li><code>1</code> for double</li>
     *                         <li><code>2</code> for float</li>
     *                         <li><code>3</code> for long</li>
     *                         <li><code>4</code> for short</li>
     *                         <li><code>5</code> for string</li>
     *                         <li><code>6</code> for character</li>
     *                         </ul>
     * @param fixed_value      may be <code>null</code>
     * @return                 index of the parameter<br>
     *                         <code>-1</code> if an error occurs
     */
    public int defineParameter(String name, String symbol, String units, String description, String format_string, int type, String fixed_value) {
 	int i, j;
	i = parameterCount + 1;
	if (name == null) {
	    setError("NULL name not allowed for parameter definition (defineParameter)");
	    return -1;
	}
	if (!SDDSUtil.isValidName(name))
	    return -1;
	for ( j=0 ; j<parameterCount ; j++ ) {
	    if (name.compareTo(parameterObject[j].getName()) == 0) {
		setError("Parameter " + name + " already exists (defineParameter)");
	        return -1;
	    }
	}
	if (!SDDSUtil.isValidType(type)) {
    	    setError("Unknown data type (defineParameter)");
    	    return -1;
	}
	if (format_string != null) {
	    if (!SDDSUtil.verifyPrintfFormat(format_string, type)) {
     		setError("Invalid format string (defineParameter)");
     		return -1;
	    }
	}
	if (parameterObject == null) {
	    parameterObject = new SDDSParameter[1];
	} else {
	    try {
		parameterObject = (SDDSParameter[])SDDSUtil.resize(parameterObject, i);
	    } catch (Exception e) {
		setError("Unable to resize because variable is not an array (defineParameter)");
	        return -1;
	    }
	}
	parameterObject[parameterCount] = new SDDSParameter();
	parameterObject[parameterCount].setupParameter(name, symbol, units, description, format_string, type, fixed_value);
        parameterCount++;
	return parameterCount-1;
    }

    /**
     * Creates a new parameter with the specified settings.
     * 
     * @param name             should be a valid name as defined by
     *                         SDDSUtil.isValidName
     * @param symbol           may be <code>null</code>
     * @param units            may be <code>null</code>
     * @param description      may be <code>null</code>
     * @param format_string    may be <code>null</code>
     * @param type             valid values are:
     *                         <ul>
     *                         <li><code>"double"</code></li>
     *                         <li><code>"float"</code></li>
     *                         <li><code>"long"</code></li>
     *                         <li><code>"short"</code></li>
     *                         <li><code>"string"</code></li>
     *                         <li><code>"character"</code></li>
     *                         </ul>
     * @param fixed_value      may be <code>null</code>
     * @return                 index of the parameter<br>
     *                         <code>-1</code> if an error occurs
     */
    public int defineParameter(String name, String symbol, String units, 
				String description, String format_string, 
				String type, String fixed_value) {
	return defineParameter(name,symbol,units,description, format_string, 
			       SDDSUtil.identifyType(type), fixed_value);
    }

    /**
     * Creates a new parameter with the specified settings.
     * 
     * @param name             should be a valid name as defined by
     *                         SDDSUtil.isValidName
     * @param type             valid values are:
     *                         <ul>
     *                         <li><code>1</code> for double</li>
     *                         <li><code>2</code> for float</li>
     *                         <li><code>3</code> for long</li>
     *                         <li><code>4</code> for short</li>
     *                         <li><code>5</code> for string</li>
     *                         <li><code>6</code> for character</li>
     *                         </ul>
     * @return                 index of the parameter<br>
     *                         <code>-1</code> if an error occurs
     */
    public int defineParameter(String name, int type) {
	return defineParameter(name,null,null,null,null,type,null);
    }

    /**
     * Creates a new parameter with the specified settings.
     * 
     * @param name             should be a valid name as defined by
     *                         SDDSUtil.isValidName
     * @param type             valid values are:
     *                         <ul>
     *                         <li><code>"double"</code></li>
     *                         <li><code>"float"</code></li>
     *                         <li><code>"long"</code></li>
     *                         <li><code>"short"</code></li>
     *                         <li><code>"string"</code></li>
     *                         <li><code>"character"</code></li>
     *                         </ul>
     * @return                 index of the parameter<br>
     *                         <code>-1</code> if an error occurs
     */
    public int defineParameter(String name, String type) {
	return defineParameter(name,null,null,null,null,SDDSUtil.identifyType(type),null);
    }

    /**
     * Creates new parameters with the specified settings.
     * 
     * @param names            should be an array with valid names as defined by
     *                         SDDSUtil.isValidName
     * @param symbols          elements may be <code>null</code>
     * @param units            elements may be <code>null</code>
     * @param descriptions     elements may be <code>null</code>
     * @param format_strings   elements may be <code>null</code>
     * @param types            valid values or the elements are:
     *                         <ul>
     *                         <li><code>1</code> for double</li>
     *                         <li><code>2</code> for float</li>
     *                         <li><code>3</code> for long</li>
     *                         <li><code>4</code> for short</li>
     *                         <li><code>5</code> for string</li>
     *                         <li><code>6</code> for character</li>
     *                         </ul>
     * @param fixed_values     elements may be <code>null</code>
     * @return                 <code>true</code> if successful<br>
     *                         <code>false</code> if an error occurs
     */
    public boolean defineParameters(String[] names, String[] symbols, String[] units, 
				   String[] descriptions, String[] format_strings, 
				   int[] types, String[] fixed_values) {
	int n, i;
	n = Array.getLength(names);
	if ((n != Array.getLength(symbols)) || (n != Array.getLength(units)) || 
	    (n != Array.getLength(descriptions)) || (n != Array.getLength(format_strings)) || 
	    (n != Array.getLength(types)) || (n != Array.getLength(fixed_values))) {
	    setError("Unequal number of array elements (defineParameters)");
	    return false;
	}
	for ( i=0 ; i < n ; i++ ) {
	    if (defineParameter(names[i],symbols[i],units[i],descriptions[i], 
				format_strings[i], types[i], 
				fixed_values[i]) == -1) {
		return false;
	    }
	}
	return true;
    }

    /**
     * Creates new parameters with the specified settings.
     * 
     * @param names            should be an array with valid names as defined by
     *                         SDDSUtil.isValidName
     * @param symbols          elements may be <code>null</code>
     * @param units            elements may be <code>null</code>
     * @param descriptions     elements may be <code>null</code>
     * @param format_strings   elements may be <code>null</code>
     * @param types            valid values or the elements are:
     *                         <ul>
     *                         <li><code>"double"</code></li>
     *                         <li><code>"float"</code></li>
     *                         <li><code>"long"</code></li>
     *                         <li><code>"short"</code></li>
     *                         <li><code>"string"</code></li>
     *                         <li><code>"character"</code></li>
     *                         </ul>
     * @param fixed_values     elements may be <code>null</code>
     * @return                 <code>true</code> if successful<br>
     *                         <code>false</code> if an error occurs
     */
    public boolean defineParameters(String[] names, String[] symbols, String[] units, 
				   String[] descriptions, String[] format_strings, 
				   String[] types, String[] fixed_values) {
	int n, i;
	n = Array.getLength(names);
	if ((n != Array.getLength(symbols)) || (n != Array.getLength(units)) || 
	    (n != Array.getLength(descriptions)) || (n != Array.getLength(format_strings)) || 
	    (n != Array.getLength(types)) || (n != Array.getLength(fixed_values))) {
	    setError("Unequal number of array elements (defineParameters)");
	    return false;
	}
	for ( i=0 ; i < n ; i++ ) {
	    if (defineParameter(names[i],symbols[i],units[i],descriptions[i], 
				format_strings[i], SDDSUtil.identifyType(types[i]), 
				fixed_values[i]) == -1) {
		return false;
	    }
	}
	return true;
    }

    /**
     * Creates new parameters with the specified settings.
     * 
     * @param names            should be an array with valid names as defined by
     *                         SDDSUtil.isValidName
     * @param types            valid values or the elements are:
     *                         <ul>
     *                         <li><code>1</code> for double</li>
     *                         <li><code>2</code> for float</li>
     *                         <li><code>3</code> for long</li>
     *                         <li><code>4</code> for short</li>
     *                         <li><code>5</code> for string</li>
     *                         <li><code>6</code> for character</li>
     *                         </ul>
     * @return                 <code>true</code> if successful<br>
     *                         <code>false</code> if an error occurs
     */
    public boolean defineParameters(String[] names, int[] types) {
	int n, i;
	n = Array.getLength(names);
	if (n != Array.getLength(types)) {
	    setError("Unequal number of array elements (defineParameters)");
	    return false;
	}
	for ( i=0 ; i < n ; i++ ) {
	    if (defineParameter(names[i],null,null,null,null,types[i],null) == -1) {
		return false;
	    }
	}
	return true;
    }

    /**
     * Creates new parameters with the specified settings.
     * 
     * @param names            should be an array with valid names as defined by
     *                         SDDSUtil.isValidName
     * @param types            valid values or the elements are:
     *                         <ul>
     *                         <li><code>"double"</code></li>
     *                         <li><code>"float"</code></li>
     *                         <li><code>"long"</code></li>
     *                         <li><code>"short"</code></li>
     *                         <li><code>"string"</code></li>
     *                         <li><code>"character"</code></li>
     *                         </ul>
     * @return                 <code>true</code> if successful<br>
     *                         <code>false</code> if an error occurs
     */
    public boolean defineParameters(String[] names, String[] types) {
	int n, i;
	n = Array.getLength(names);
	if (n != Array.getLength(types)) {
	    setError("Unequal number of array elements (defineParameters)");
	    return false;
	}
	for ( i=0 ; i < n ; i++ ) {
	    if (defineParameter(names[i],null,null,null,null,SDDSUtil.identifyType(types[i]),null) == -1) {
		return false;
	    }
	}
	return true;
    }

    /**
     * Creates a new array with the specified settings.
     * 
     * @param name             should be a valid name as defined by
     *                         SDDSUtil.isValidName
     * @param symbol           may be <code>null</code>
     * @param units            may be <code>null</code>
     * @param description      may be <code>null</code>
     * @param format_string    may be <code>null</code>
     * @param group_name       may be <code>null</code>
     * @param type             valid values are:
     *                         <ul>
     *                         <li><code>1</code> for double</li>
     *                         <li><code>2</code> for float</li>
     *                         <li><code>3</code> for long</li>
     *                         <li><code>4</code> for short</li>
     *                         <li><code>5</code> for string</li>
     *                         <li><code>6</code> for character</li>
     *                         </ul>
     * @param field_length     has no effect currently
     * @param dimensions       integer
     * @return                 <code>-1</code>  if error occurs <br>
     *                         index of the array
     */
    public int defineArray(String name, String symbol, String units, String description,
			       String format_string, String group_name, int type, int field_length, int dimensions)
    {
	int i, j;
	i = arrayCount + 1;
	if (name == null){
	    setError("NULL name not allowed for array definition (defineArray)");
	    return -1;
	}
	if (!SDDSUtil.isValidName(name))
	    return -1;
	for (j=0;j<arrayCount;j++){
	    if (name.compareTo(arrayObject[j].getName()) == 0){
		setError("Array " + name + " already exists (defineArray)");
		return -1;
	    }
	}
	if (!SDDSUtil.isValidType(type)){
	    setError("Unknown data type (defineArray)");
	    return -1;
	}
	if (format_string != null){
	    if (!SDDSUtil.verifyPrintfFormat(format_string, type)){
		setError("Invalid format string (defineArray)");
		return -1;
	    }
	}
	if ((field_length < 0) && (type != SDDS_STRING)) {
	    setError("Invalid field length (defineArray)");
	    return -1;
	}
	if (dimensions < 0) {
	    setError("Invalid dimensions value (defineArray)");
	    return -1;
	}
	if (arrayObject == null){
	    arrayObject = new SDDSArray[1];
	}
	else {
	    try {
		arrayObject = (SDDSArray[])SDDSUtil.resize(arrayObject, i);
	    }
	    catch (Exception e){
		setError("Unable to resize because variable is not an array (defineArray)");
		return -1;
	    }
	}
	arrayObject[arrayCount] = new SDDSArray();
	arrayObject[arrayCount].setupArray(name, symbol, units, description, format_string, group_name, type, field_length, dimensions);
	arrayCount++;
	return arrayCount-1;
    }

    /**
     * Creates a new array with the specified settings.
     * 
     * @param name             should be a valid name as defined by
     *                         SDDSUtil.isValidName
     * @param symbol           may be <code>null</code>
     * @param units            may be <code>null</code>
     * @param description      may be <code>null</code>
     * @param format_string    may be <code>null</code>
     * @param group_name       may be <code>null</code>
     * @param type             valid values are:
     *                         <ul>
     *                         <li><code>"double"</code> for double</li>
     *                         <li><code>"float"</code> for float</li>
     *                         <li><code>"long"</code> for long</li>
     *                         <li><code>"short"</code> for short</li>
     *                         <li><code>"string"</code> for string</li>
     *                         <li><code>"character"</code> for character</li>
     *                         </ul>
     * @param field_length     has no effect currently
     * @param dimensions       integer
     * @return                 index of the array<br>
     *                         <code>-1</code> if an error occurs
     */
    public int defineArray(String name, String symbol, String units, String description,
			       String format_string, String group_name, String type, int field_length, int dimensions)
    {
	return defineArray(name, symbol, units, description, format_string,
			   group_name, SDDSUtil.identifyType(type), field_length, dimensions);
    }

    /**
     * Creates a new array with the specified settings.
     * 
     * @param name             should be a valid name as defined by
     *                         SDDSUtil.isValidName
     * @param type             valid values are:
     *                         <ul>
     *                         <li><code>"double"</code> for double</li>
     *                         <li><code>"float"</code> for float</li>
     *                         <li><code>"long"</code> for long</li>
     *                         <li><code>"short"</code> for short</li>
     *                         <li><code>"string"</code> for string</li>
     *                         <li><code>"character"</code> for character</li>
     *                         </ul>
     * @param dimensions       integer
     * @return                 index of the array<br>
     *                         <code>-1</code> if an error occurs
     */
    public int defineArray(String name, int type, int dimensions)
    {
	return defineArray(name, null, null, null, null, null, type, 0, dimensions);
    }

    /**
     * Creates a new array with the specified settings.
     * 
     * @param name             should be a valid name as defined by
     *                         SDDSUtil.isValidName
     * @param type             valid values are:
     *                         <ul>
     *                         <li><code>1</code> for double</li>
     *                         <li><code>2</code> for float</li>
     *                         <li><code>3</code> for long</li>
     *                         <li><code>4</code> for short</li>
     *                         <li><code>5</code> for string</li>
     *                         <li><code>6</code> for character</li>
     *                         </ul>
     * @param dimensions       integer
     * @return                 index of the array<br>
     *                         <code>-1</code> if an error occurs
     */
    public int defineArray(String name, String type, int dimensions)
    {
	return defineArray(name, null, null, null, null, null, SDDSUtil.identifyType(type), 0, dimensions);
    }

    /**
     * Creates new arrays with the specified settings.
     * 
     * @param names            should be an array with valid names as defined by
     *                         SDDSUtil.isValidName
     * @param symbols          elements may be <code>null</code>
     * @param units            elements may be <code>null</code>
     * @param descriptions     elements may be <code>null</code>
     * @param format_strings   elements may be <code>null</code>
     * @param group_names      elements may be <code>null</code>
     * @param types            valid values or the elements are:
     *                         <ul>
     *                         <li><code>1</code> for double</li>
     *                         <li><code>2</code> for float</li>
     *                         <li><code>3</code> for long</li>
     *                         <li><code>4</code> for short</li>
     *                         <li><code>5</code> for string</li>
     *                         <li><code>6</code> for character</li>
     *                         </ul>
     * @param field_lengths    elements have no effect currently
     * @param dimensions       integer elements
     * @return                 <code>true</code> if successful<br>
     *                         <code>false</code> if an error occurs
     */
    public boolean defineArrays(String[] names, String[] symbols, String[] units, 
				   String[] descriptions, String[] format_strings, 
				    String[] group_names, int[] types, int[] field_lengths, int[] dimensions) {
	int n, i;
	n = Array.getLength(names);
	if ((n != Array.getLength(symbols)) || (n != Array.getLength(units)) || 
	    (n != Array.getLength(descriptions)) || (n != Array.getLength(format_strings)) || 
	    (n != Array.getLength(group_names)) || (n != Array.getLength(field_lengths)) || 
	    (n != Array.getLength(types)) || (n != Array.getLength(dimensions))) {
	    setError("Unequal number of array elements (defineArrays)");
	    return false;
	}
	for ( i=0 ; i < n ; i++ ) {
	    if (defineArray(names[i],symbols[i],units[i],descriptions[i], 
			    format_strings[i], group_names[i], types[i], field_lengths[i], dimensions[i]) == -1) {
		return false;
	    }
	}
	return true;
    }

    /**
     * Creates new arrays with the specified settings.
     * 
     * @param names            should be an array with valid names as defined by
     *                         SDDSUtil.isValidName
     * @param symbols          elements may be <code>null</code>
     * @param units            elements may be <code>null</code>
     * @param descriptions     elements may be <code>null</code>
     * @param format_strings   elements may be <code>null</code>
     * @param group_names      elements may be <code>null</code>
     * @param types            valid values or the elements are:
     *                         <ul>
     *                         <li><code>"double"</code></li>
     *                         <li><code>"float"</code></li>
     *                         <li><code>"long"</code></li>
     *                         <li><code>"short"</code></li>
     *                         <li><code>"string"</code></li>
     *                         <li><code>"character"</code></li>
     *                         </ul>
     * @param field_lengths    elements have no effect currently
     * @param dimensions       integer elements
     * @return                 <code>true</code> if successful<br>
     *                         <code>false</code> if an error occurs
     */
    public boolean defineArrays(String[] names, String[] symbols, String[] units, 
				   String[] descriptions, String[] format_strings, 
				   String[] group_names, String[] types, int[] field_lengths, int dimensions[]) {
	int n, i;
	n = Array.getLength(names);
	if ((n != Array.getLength(symbols)) || (n != Array.getLength(units)) || 
	    (n != Array.getLength(descriptions)) || (n != Array.getLength(format_strings)) || 
	    (n != Array.getLength(group_names)) || (n != Array.getLength(field_lengths)) || 
	    (n != Array.getLength(types)) || (n != Array.getLength(dimensions))) {
	    setError("Unequal number of array elements (defineArrays)");
	    return false;
	}
	for ( i=0 ; i < n ; i++ ) {
	    if (defineArray(names[i],symbols[i],units[i],descriptions[i], 
  			    format_strings[i], group_names[i], 
			    SDDSUtil.identifyType(types[i]), field_lengths[i], dimensions[i]) == -1) {
		return false;
	    }
	}
	return true;
    }

    /**
     * Creates new arrays with the specified settings.
     * 
     * @param names            should be an array with valid names as defined by
     *                         SDDSUtil.isValidName
     * @param types            valid values or the elements are:
     *                         <ul>
     *                         <li><code>1</code> for double</li>
     *                         <li><code>2</code> for float</li>
     *                         <li><code>3</code> for long</li>
     *                         <li><code>4</code> for short</li>
     *                         <li><code>5</code> for string</li>
     *                         <li><code>6</code> for character</li>
     *                         </ul>
     * @param dimensions       integer elements
     * @return                 <code>true</code> if successful<br>
     *                         <code>false</code> if an error occurs
     */
    public boolean defineArrays(String[] names, int[] types, int dimensions[]) {
	int n, i;
	n = Array.getLength(names);
	if ((n != Array.getLength(types)) || (n != Array.getLength(dimensions))) {
	    setError("Unequal number of array elements (defineArrays)");
	    return false;
	}
	for ( i=0 ; i < n ; i++ ) {
	    if (defineArray(names[i],null,null,null,null,null,types[i],0, dimensions[i]) == -1) {
		return false;
	    }
	}
	return true;
    }

    /**
     * Creates new arrays with the specified settings.
     * 
     * @param names            should be an array with valid names as defined by
     *                         SDDSUtil.isValidName
     * @param types            valid values or the elements are:
     *                         <ul>
     *                         <li><code>"double"</code></li>
     *                         <li><code>"float"</code></li>
     *                         <li><code>"long"</code></li>
     *                         <li><code>"short"</code></li>
     *                         <li><code>"string"</code></li>
     *                         <li><code>"character"</code></li>
     *                         </ul>
     * @param dimensions       integer elements
     * @return                 <code>true</code> if successful<br>
     *                         <code>false</code> if an error occurs
     */
    public boolean defineArrays(String[] names, String[] types, int[] dimensions) {
	int n, i;
	n = Array.getLength(names);
	if ((n != Array.getLength(types)) || (n != Array.getLength(dimensions))) {
	    setError("Unequal number of array elements (defineArrays)");
	    return false;
	}
	for ( i=0 ; i < n ; i++ ) {
	    if (defineArray(names[i],null,null,null,null,null,SDDSUtil.identifyType(types[i]),0,dimensions[i]) == -1) {
		return false;
	    }
	}
	return true;
    }
    
    /**
     * Creates a new column with the specified settings.
     * 
     * @param name             should be a valid name as defined by
     *                         SDDSUtil.isValidName
     * @param symbol           may be <code>null</code>
     * @param units            may be <code>null</code>
     * @param description      may be <code>null</code>
     * @param format_string    may be <code>null</code>
     * @param type             valid values are:
     *                         <ul>
     *                         <li><code>1</code> for double</li>
     *                         <li><code>2</code> for float</li>
     *                         <li><code>3</code> for long</li>
     *                         <li><code>4</code> for short</li>
     *                         <li><code>5</code> for string</li>
     *                         <li><code>6</code> for character</li>
     *                         </ul>
     * @param field_length     has no effect currently
     * @return                 index of the column<br>
     *                         <code>-1</code> if an error occurs
     */
    public int defineColumn(String name, String symbol, String units, String description, 
			     String format_string, int type, int field_length) {
 	int i, j;
	i = columnCount + 1;
	if (name == null) {
	    setError("NULL name not allowed for column definition (defineColumn)");
	    return -1;
	}
	if (!SDDSUtil.isValidName(name))
	    return -1;
	for ( j=0 ; j<columnCount ; j++ ) {
	    if (name.compareTo(columnObject[j].getName()) == 0) {
		setError("Column " + name + " already exists (defineColumn)");
	        return -1;
	    }
	}
	if (!SDDSUtil.isValidType(type)) {
    	    setError("Unknown data type (defineColumn)");
    	    return -1;
	}
	if (format_string != null) {
	    if (!SDDSUtil.verifyPrintfFormat(format_string, type)) {
     		setError("Invalid format string (defineColumn)");
     		return -1;
	    }
	}
	if ((field_length < 0) && (type != SDDS_STRING)) {
	    setError("Invalid field length (defineColumn)");
	    return -1;
	}
	if (columnObject == null) {
	    columnObject = new SDDSColumn[1];
	} else {
	    try {
		columnObject = (SDDSColumn[])SDDSUtil.resize(columnObject, i);
	    } catch (Exception e) {
		setError("Unable to resize because variable is not an array (defineColumn)");
	        return -1;
	    }
	}
	columnObject[columnCount] = new SDDSColumn();
	columnObject[columnCount].setupColumn(name, symbol, units, description, format_string, type, field_length);
        columnCount++;
	return columnCount-1;
    }

    /**
     * Creates a new column with the specified settings.
     * 
     * @param name             should be a valid name as defined by
     *                         SDDSUtil.isValidName
     * @param symbol           may be <code>null</code>
     * @param units            may be <code>null</code>
     * @param description      may be <code>null</code>
     * @param format_string    may be <code>null</code>
     * @param type             valid values are:
     *                         <ul>
     *                         <li><code>"double"</code></li>
     *                         <li><code>"float"</code></li>
     *                         <li><code>"long"</code></li>
     *                         <li><code>"short"</code></li>
     *                         <li><code>"string"</code></li>
     *                         <li><code>"character"</code></li>
     *                         </ul>
     * @param field_length     has no effect currently
     * @return                 index of the column<br>
     *                         <code>-1</code> if an error occurs
     */
    public int defineColumn(String name, String symbol, String units, String description, String format_string, String type, int field_length) {
	return defineColumn(name,symbol,units,description, format_string, SDDSUtil.identifyType(type), field_length);
    }

    /**
     * Creates a new column with the specified settings.
     * 
     * @param name             should be a valid name as defined by
     *                         SDDSUtil.isValidName
     * @param type             valid values are:
     *                         <ul>
     *                         <li><code>1</code> for double</li>
     *                         <li><code>2</code> for float</li>
     *                         <li><code>3</code> for long</li>
     *                         <li><code>4</code> for short</li>
     *                         <li><code>5</code> for string</li>
     *                         <li><code>6</code> for character</li>
     *                         </ul>
     * @return                 index of the column<br>
     *                         <code>-1</code> if an error occurs
     */
    public int defineColumn(String name, int type) {
	return defineColumn(name,null,null,null,null,type,0);
    }

    /**
     * Creates a new column with the specified settings.
     * 
     * @param name             should be a valid name as defined by
     *                         SDDSUtil.isValidName
     * @param type             valid values are:
     *                         <ul>
     *                         <li><code>"double"</code></li>
     *                         <li><code>"float"</code></li>
     *                         <li><code>"long"</code></li>
     *                         <li><code>"short"</code></li>
     *                         <li><code>"string"</code></li>
     *                         <li><code>"character"</code></li>
     *                         </ul>
     * @return                 index of the column<br>
     *                         <code>-1</code> if an error occurs
     */
    public int defineColumn(String name, String type) {
	return defineColumn(name,null,null,null,null,SDDSUtil.identifyType(type),0);
    }

    /**
     * Creates new columns with the specified settings.
     * 
     * @param names            should be an array with valid names as defined by
     *                         SDDSUtil.isValidName
     * @param symbols          elements may be <code>null</code>
     * @param units            elements may be <code>null</code>
     * @param descriptions     elements may be <code>null</code>
     * @param format_strings   elements may be <code>null</code>
     * @param types            valid values or the elements are:
     *                         <ul>
     *                         <li><code>1</code> for double</li>
     *                         <li><code>2</code> for float</li>
     *                         <li><code>3</code> for long</li>
     *                         <li><code>4</code> for short</li>
     *                         <li><code>5</code> for string</li>
     *                         <li><code>6</code> for character</li>
     *                         </ul>
     * @param field_lengths    has no effect currently
     * @return                 <code>true</code> if successful<br>
     *                         <code>false</code> if an error occurs
     */
    public boolean defineColumns(String[] names, String[] symbols, String[] units, 
				   String[] descriptions, String[] format_strings, 
				   int[] types, int[] field_lengths) {
	int n, i;
	n = Array.getLength(names);
	if ((n != Array.getLength(symbols)) || (n != Array.getLength(units)) || 
	    (n != Array.getLength(descriptions)) || (n != Array.getLength(format_strings)) || 
	    (n != Array.getLength(types)) || (n != Array.getLength(field_lengths))) {
	    setError("Unequal number of array elements (defineColumns)");
	    return false;
	}
	for ( i=0 ; i < n ; i++ ) {
	    if (defineColumn(names[i],symbols[i],units[i],descriptions[i], 
				format_strings[i], types[i], 
				field_lengths[i]) == -1) {
		return false;
	    }
	}
	return true;
    }

    /**
     * Creates new columns with the specified settings.
     * 
     * @param names            should be an array with valid names as defined by
     *                         SDDSUtil.isValidName
     * @param symbols          elements may be <code>null</code>
     * @param units            elements may be <code>null</code>
     * @param descriptions     elements may be <code>null</code>
     * @param format_strings   elements may be <code>null</code>
     * @param types            valid values or the elements are:
     *                         <ul>
     *                         <li><code>"double"</code></li>
     *                         <li><code>"float"</code></li>
     *                         <li><code>"long"</code></li>
     *                         <li><code>"short"</code></li>
     *                         <li><code>"string"</code></li>
     *                         <li><code>"character"</code></li>
     *                         </ul>
     * @param field_lengths    has no effect currently
     * @return                 <code>true</code> if successful<br>
     *                         <code>false</code> if an error occurs
     */
    public boolean defineColumns(String[] names, String[] symbols, String[] units, 
				   String[] descriptions, String[] format_strings, 
				   String[] types, int[] field_lengths) {
	int n, i;
	n = Array.getLength(names);
	if ((n != Array.getLength(symbols)) || (n != Array.getLength(units)) || 
	    (n != Array.getLength(descriptions)) || (n != Array.getLength(format_strings)) || 
	    (n != Array.getLength(types)) || (n != Array.getLength(field_lengths))) {
	    setError("Unequal number of array elements (defineColumns)");
	    return false;
	}
	for ( i=0 ; i < n ; i++ ) {
	    if (defineColumn(names[i],symbols[i],units[i],descriptions[i], 
				format_strings[i], SDDSUtil.identifyType(types[i]), 
				field_lengths[i]) == -1) {
		return false;
	    }
	}
	return true;
    }

    /**
     * Creates new columns with the specified settings.
     * 
     * @param names            should be an array with valid names as defined by
     *                         SDDSUtil.isValidName
     * @param types            valid values or the elements are:
     *                         <ul>
     *                         <li><code>1</code> for double</li>
     *                         <li><code>2</code> for float</li>
     *                         <li><code>3</code> for long</li>
     *                         <li><code>4</code> for short</li>
     *                         <li><code>5</code> for string</li>
     *                         <li><code>6</code> for character</li>
     *                         </ul>
     * @return                 <code>true</code> if successful<br>
     *                         <code>false</code> if an error occurs
     */
    public boolean defineColumns(String[] names, int[] types) {
	int n, i;
	n = Array.getLength(names);
	if (n != Array.getLength(types)) {
	    setError("Unequal number of array elements (defineColumns)");
	    return false;
	}
	for ( i=0 ; i < n ; i++ ) {
	    if (defineColumn(names[i],null,null,null,null,types[i],0) == -1) {
		return false;
	    }
	}
	return true;
    }

    /**
     * Creates new columns with the specified settings.
     * 
     * @param names            should be an array with valid names as defined by
     *                         SDDSUtil.isValidName
     * @param types            valid values or the elements are:
     *                         <ul>
     *                         <li><code>"double"</code></li>
     *                         <li><code>"float"</code></li>
     *                         <li><code>"long"</code></li>
     *                         <li><code>"short"</code></li>
     *                         <li><code>"string"</code></li>
     *                         <li><code>"character"</code></li>
     *                         </ul>
     * @return                 <code>true</code> if successful<br>
     *                         <code>false</code> if an error occurs
     */
    public boolean defineColumns(String[] names, String[] types) {
	int n, i;
	n = Array.getLength(names);
	if (n != Array.getLength(types)) {
	    setError("Unequal number of array elements (defineColumns)");
	    return false;
	}
	for ( i=0 ; i < n ; i++ ) {
	    if (defineColumn(names[i],null,null,null,null,SDDSUtil.identifyType(types[i]),0) == -1) {
		return false;
	    }
	}
	return true;
    }  

    /**
     * Return the index of the specified parameter.
     * 
     * @param name             name of an existing parameter
     * @return                 index of parameter<br>
     *                         <code>-1</code> if the parameter does not exist
     */
    public int getParameterIndex(String name) {
	int i;
	for ( i=0 ; i<parameterCount ; i++ )
	    if (name.compareTo(parameterObject[i].getName()) == 0)
		return i;
	setError("parameter " + name + " does not exist (getParameterIndex)");
	return -1;
    }

    /**
     * Return the indexes of the specified parameter.
     * 
     * @param names            names of an existing parameters
     * @return                 indexes of parameters<br>
     *                         elements may contain <code>-1</code> if the parameter does not exist
     */
    public int[] getParameterIndexes(String[] names) {
	int[] n;
	int i, j, k;
	j = Array.getLength(names);
	n = new int[j];
	for ( k=0 ; k < j ; k++) {
	    n[k] = -1;
	    for ( i=0 ; i<parameterCount ; i++ )
		if (names[k].compareTo(parameterObject[i].getName()) == 0)
		    n[k] = i;
	}
	return n;
    }

    /**
     * Return the index of the specified array.
     * 
     * @param name             name of an existing array
     * @return                 index of array<br>
     *                         <code>-1</code> if the array does not exist
     */
    public int getArrayIndex(String name) {
	int i;
	for ( i=0 ; i<arrayCount ; i++ )
	    if (name.compareTo(arrayObject[i].getName()) == 0)
		return i;
	setError("array " + name + " does not exist (getArrayIndex)");
	return -1;
    }

    /**
     * Return the indexes of the specified arrays.
     * 
     * @param names            names of an existing arrays
     * @return                 indexes of arrays<br>
     *                         elements may contain <code>-1</code> if the array does not exist
     */
    public int[] getArrayIndexes(String[] names) {
	int[] n;
	int i, j, k;
	j = Array.getLength(names);
	n = new int[j];
	for ( k=0 ; k < j ; k++) {
	    n[k] = -1;
	    for ( i=0 ; i<arrayCount ; i++ )
		if (names[k].compareTo(arrayObject[i].getName()) == 0)
		    n[k] = i;
	}
	return n;
    }

    /**
     * Return the index of the specified column.
     * 
     * @param name             name of an existing column
     * @return                 index of column<br>
     *                         <code>-1</code> if the column does not exist
     */
    public int getColumnIndex(String name) {
	int i;
	for ( i=0 ; i<columnCount ; i++ )
	    if (name.compareTo(columnObject[i].getName()) == 0)
		return i;
	setError("column " + name + " does not exist (getColumnIndex)");
	return -1;
    }

    /**
     * Return the indexes of the specified columns.
     * 
     * @param names            names of an existing columns
     * @return                 indexes of columns<br>
     *                         elements may contain <code>-1</code> if the column does not exist
     */
    public int[] getColumnIndexes(String[] names) {
	int[] n;
	int i, j, k;
	j = Array.getLength(names);
	n = new int[j];
	for ( k=0 ; k < j ; k++) {
	    n[k] = -1;
	    for ( i=0 ; i<columnCount ; i++ )
		if (names[k].compareTo(columnObject[i].getName()) == 0)
		    n[k] = i;
	}
	return n;
    }


    /**
     * Set the values of the parameter starting from the specified page.
     * 
     * @param name             name of an existing parameter
     * @param values           array of values
     * @param startPage        page to start setting values
     * @return                 <code>true</code> if successful
     *                         <code>false</code> if index does not exist
     */
    public boolean setParameter(String name, short[] values, int startPage) {
	return setParameter(getParameterIndex(name), SDDSUtil.castArrayAsLong(values), startPage);
    }

    /**
     * Set the values of the parameter starting from the specified page.
     * 
     * @param parameterIndex   index of an existing parameter
     * @param values           array of values
     * @param startPage        page to start setting values
     * @return                 <code>true</code> if successful
     *                         <code>false</code> if index does not exist
     */
    public boolean setParameter(int parameterIndex, short[] values, int startPage) {
	return setParameter(parameterIndex, SDDSUtil.castArrayAsLong(values), startPage);
    }

    /**
     * Set the values of the parameter starting from the specified page.
     * 
     * @param name             name of an existing parameter
     * @param values           array of values
     * @param startPage        page to start setting values
     * @return                 <code>true</code> if successful
     *                         <code>false</code> if index does not exist
     */
    public boolean setParameter(String name, int[] values, int startPage) {
	return setParameter(getParameterIndex(name), SDDSUtil.castArrayAsLong(values), startPage);
    }

    /**
     * Set the values of the parameter starting from the specified page.
     * 
     * @param parameterIndex   index of an existing parameter
     * @param values           array of values
     * @param startPage        page to start setting values
     * @return                 <code>true</code> if successful
     *                         <code>false</code> if index does not exist
     */
    public boolean setParameter(int parameterIndex, int[] values, int startPage) {
	return setParameter(parameterIndex, SDDSUtil.castArrayAsLong(values), startPage);
    }

    /**
     * Set the values of the parameter starting from the specified page.
     * 
     * @param name             name of an existing parameter
     * @param values           array of values
     * @param startPage        page to start setting values
     * @return                 <code>true</code> if successful
     *                         <code>false</code> if index does not exist
     */
    public boolean setParameter(String name, long[] values, int startPage) {
	return setParameter(getParameterIndex(name), values, startPage);
    }

    /**
     * Set the values of the parameter starting from the specified page.
     * 
     * @param parameterIndex   index of an existing parameter
     * @param values           array of values
     * @param startPage        page to start setting values
     * @return                 <code>true</code> if successful
     *                         <code>false</code> if index does not exist
     */
    public boolean setParameter(int parameterIndex, long[] values, int startPage) {
	int i;
	if ((parameterIndex < 0) || (parameterIndex >= parameterCount)) {
	    setError("parameter index " + parameterIndex + " does not exist (setParameter)");
	    return false;
	}
	for ( i=0 ; i < Array.getLength(values) ; i++)
	    parameterObject[parameterIndex].setValue(values[i], i+startPage);
	return true;
    }

    /**
     * Set the values of the parameter starting from the specified page.
     * 
     * @param name             name of an existing parameter
     * @param values           array of values
     * @param startPage        page to start setting values
     * @return                 <code>true</code> if successful
     *                         <code>false</code> if index does not exist
     */
    public boolean setParameter(String name, float[] values, int startPage) {
	return setParameter(getParameterIndex(name), SDDSUtil.castArrayAsDouble(values), startPage);
    }	

    /**
     * Set the values of the parameter starting from the specified page.
     * 
     * @param parameterIndex   index of an existing parameter
     * @param values           array of values
     * @param startPage        page to start setting values
     * @return                 <code>true</code> if successful
     *                         <code>false</code> if index does not exist
     */
    public boolean setParameter(int parameterIndex, float[] values, int startPage) {
	return setParameter(parameterIndex, SDDSUtil.castArrayAsDouble(values), startPage);
    }

    /**
     * Set the values of the parameter starting from the specified page.
     * 
     * @param name             name of an existing parameter
     * @param values           array of values
     * @param startPage        page to start setting values
     * @return                 <code>true</code> if successful
     *                         <code>false</code> if index does not exist
     */
    public boolean setParameter(String name, double[] values, int startPage) {
	return setParameter(getParameterIndex(name), values, startPage);
    }	

    /**
     * Set the values of the parameter starting from the specified page.
     * 
     * @param parameterIndex   index of an existing parameter
     * @param values           array of values
     * @param startPage        page to start setting values
     * @return                 <code>true</code> if successful
     *                         <code>false</code> if index does not exist
     */
    public boolean setParameter(int parameterIndex, double[] values, int startPage) {
	int i;
	if ((parameterIndex < 0) || (parameterIndex >= parameterCount)) {
	    setError("parameter index " + parameterIndex + " does not exist (setParameter)");
	    return false;
	}
	for ( i=0 ; i < Array.getLength(values) ; i++)
	    parameterObject[parameterIndex].setValue(values[i], i+startPage);
	return true;
    }

    /**
     * Set the values of the parameter starting from the specified page.
     * 
     * @param name             name of an existing parameter
     * @param values           array of values
     * @param startPage        page to start setting values
     * @return                 <code>true</code> if successful
     *                         <code>false</code> if index does not exist
     */
    public boolean setParameter(String name, String[] values, int startPage) {
	return setParameter(getParameterIndex(name), values, startPage);
    }	

    /**
     * Set the values of the parameter starting from the specified page.
     * 
     * @param parameterIndex   index of an existing parameter
     * @param values           array of values
     * @param startPage        page to start setting values
     * @return                 <code>true</code> if successful
     *                         <code>false</code> if index does not exist
     */
    public boolean setParameter(int parameterIndex, String[] values, int startPage) {
	int i;
	if ((parameterIndex < 0) || (parameterIndex >= parameterCount)) {
	    setError("parameter index " + parameterIndex + " does not exist (setParameter)");
	    return false;
	}
	for ( i=0 ; i < Array.getLength(values) ; i++)
	    parameterObject[parameterIndex].setValue(values[i], i+startPage);
	return true;
    }

    /**
     * Set the values of the parameter starting from the specified page.
     * 
     * @param name             name of an existing parameter
     * @param values           array of values
     * @param startPage        page to start setting values
     * @return                 <code>true</code> if successful
     *                         <code>false</code> if index does not exist
     */
    public boolean setParameter(String name, char[] values, int startPage) {
	return setParameter(getParameterIndex(name), SDDSUtil.castArrayAsString(values), startPage);
    }	

    /**
     * Set the values of the parameter starting from the specified page.
     * 
     * @param parameterIndex   index of an existing parameter
     * @param values           array of values
     * @param startPage        page to start setting values
     * @return                 <code>true</code> if successful
     *                         <code>false</code> if index does not exist
     */
    public boolean setParameter(int parameterIndex, char[] values, int startPage) {
	return setParameter(parameterIndex, SDDSUtil.castArrayAsString(values), startPage);
    }

    /**
     * Set the value of the parameter at the specified page.
     * 
     * @param name             name of an existing parameter
     * @param value            value to be set
     * @param page             page to set value
     * @return                 <code>true</code> if successful
     *                         <code>false</code> if index does not exist
     */
    public boolean setParameter(String name, short value, int page) {
	return setParameter(getParameterIndex(name), (long)value, page);
    }

    /**
     * Set the value of the parameter at the specified page.
     * 
     * @param parameterIndex   index of an existing parameter
     * @param value            value to be set
     * @param page             page to set value
     * @return                 <code>true</code> if successful
     *                         <code>false</code> if index does not exist
     */
    public boolean setParameter(int parameterIndex, short value, int page) {
	return setParameter(parameterIndex, (long)value, page);
    }

    /**
     * Set the value of the parameter at the specified page.
     * 
     * @param name             name of an existing parameter
     * @param value            value to be set
     * @param page             page to set value
     * @return                 <code>true</code> if successful
     *                         <code>false</code> if index does not exist
     */
    public boolean setParameter(String name, int value, int page) {
	return setParameter(getParameterIndex(name), (long)value, page);
    }

    /**
     * Set the value of the parameter at the specified page.
     * 
     * @param parameterIndex   index of an existing parameter
     * @param value            value to be set
     * @param page             page to set value
     * @return                 <code>true</code> if successful
     *                         <code>false</code> if index does not exist
     */
    public boolean setParameter(int parameterIndex, int value, int page) {
	return setParameter(parameterIndex, (long)value, page);
    }

    /**
     * Set the value of the parameter at the specified page.
     * 
     * @param name             name of an existing parameter
     * @param value            value to be set
     * @param page             page to set value
     * @return                 <code>true</code> if successful
     *                         <code>false</code> if index does not exist
     */
    public boolean setParameter(String name, long value, int page) {
	return setParameter(getParameterIndex(name), value, page);
    }

    /**
     * Set the value of the parameter at the specified page.
     * 
     * @param parameterIndex   index of an existing parameter
     * @param value            value to be set
     * @param page             page to set value
     * @return                 <code>true</code> if successful
     *                         <code>false</code> if index does not exist
     */
    public boolean setParameter(int parameterIndex, long value, int page) {
	long[] v = new long[1];
	v[0] = value;
	return setParameter(parameterIndex, v, page);
    }

    /**
     * Set the value of the parameter at the specified page.
     * 
     * @param name             name of an existing parameter
     * @param value            value to be set
     * @param page             page to set value
     * @return                 <code>true</code> if successful
     *                         <code>false</code> if index does not exist
     */
    public boolean setParameter(String name, float value, int page) {
	return setParameter(getParameterIndex(name), (double)value, page);
    }

    /**
     * Set the value of the parameter at the specified page.
     * 
     * @param parameterIndex   index of an existing parameter
     * @param value            value to be set
     * @param page             page to set value
     * @return                 <code>true</code> if successful
     *                         <code>false</code> if index does not exist
     */
    public boolean setParameter(int parameterIndex, float value, int page) {
	return setParameter(parameterIndex, (double)value, page);
    }

    /**
     * Set the value of the parameter at the specified page.
     * 
     * @param name             name of an existing parameter
     * @param value            value to be set
     * @param page             page to set value
     * @return                 <code>true</code> if successful
     *                         <code>false</code> if index does not exist
     */
    public boolean setParameter(String name, double value, int page) {
	return setParameter(getParameterIndex(name), value, page);
    }

    /**
     * Set the value of the parameter at the specified page.
     * 
     * @param parameterIndex   index of an existing parameter
     * @param value            value to be set
     * @param page             page to set value
     * @return                 <code>true</code> if successful
     *                         <code>false</code> if index does not exist
     */
    public boolean setParameter(int parameterIndex, double value, int page) {
	double[] v = new double[1];
	v[0] = value;
	return setParameter(parameterIndex, v, page);
    }

    /**
     * Set the value of the parameter at the specified page.
     * 
     * @param name             name of an existing parameter
     * @param value            value to be set
     * @param page             page to set value
     * @return                 <code>true</code> if successful
     *                         <code>false</code> if index does not exist
     */
    public boolean setParameter(String name, String value, int page) {
	return setParameter(getParameterIndex(name), value, page);
    }

    /**
     * Set the value of the parameter at the specified page.
     * 
     * @param parameterIndex   index of an existing parameter
     * @param value            value to be set
     * @param page             page to set value
     * @return                 <code>true</code> if successful
     *                         <code>false</code> if index does not exist
     */
    public boolean setParameter(int parameterIndex, String value, int page) {
	String[] v = new String[1];
	v[0] = value;
	return setParameter(parameterIndex, v, page);
    }

    /**
     * Set the value of the parameter at the specified page.
     * 
     * @param name             name of an existing parameter
     * @param value            value to be set
     * @param page             page to set value
     * @return                 <code>true</code> if successful
     *                         <code>false</code> if index does not exist
     */
    public boolean setParameter(String name, char value, int page) {
	return setParameter(getParameterIndex(name), value, page);
    }

    /**
     * Set the value of the parameter at the specified page.
     * 
     * @param parameterIndex   index of an existing parameter
     * @param value            value to be set
     * @param page             page to set value
     * @return                 <code>true</code> if successful
     *                         <code>false</code> if index does not exist
     */
    public boolean setParameter(int parameterIndex, char value, int page) {
	char[] v = new char[1];
	v[0] = value;
	return setParameter(parameterIndex, v, page);
    }




    /**
     * Set the values of the parameters at the specified page.
     * 
     * @param names            names of an existing parameters
     * @param values           array of values to be set
     * @param page             page to set values
     * @return                 <code>true</code> if successful
     *                         <code>false</code> if an error occurs
     */
    public boolean setParameters(String[] names, short[] values, int page) {
	return setParameters(getParameterIndexes(names), SDDSUtil.castArrayAsLong(values), page);
    }

    /**
     * Set the values of the parameters at the specified page.
     * 
     * @param parameterIndexes  indexes of an existing parameters
     * @param values            array of values to be set
     * @param page              page to set values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setParameters(int[] parameterIndexes, short[] values, int page) {
	return setParameters(parameterIndexes, SDDSUtil.castArrayAsLong(values), page);
    }

    /**
     * Set the values of the parameters at the specified page.
     * 
     * @param names            names of an existing parameters
     * @param values           array of values to be set
     * @param page             page to set values
     * @return                 <code>true</code> if successful
     *                         <code>false</code> if an error occurs
     */
    public boolean setParameters(String[] names, int[] values, int page) {
	return setParameters(getParameterIndexes(names), SDDSUtil.castArrayAsLong(values), page);
    }

    /**
     * Set the values of the parameters at the specified page.
     * 
     * @param parameterIndexes  indexes of an existing parameters
     * @param values            array of values to be set
     * @param page              page to set values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setParameters(int[] parameterIndexes, int[] values, int page) {
	return setParameters(parameterIndexes, SDDSUtil.castArrayAsLong(values), page);
    }

    /**
     * Set the values of the parameters at the specified page.
     * 
     * @param names            names of an existing parameters
     * @param values           array of values to be set
     * @param page             page to set values
     * @return                 <code>true</code> if successful
     *                         <code>false</code> if an error occurs
     */
    public boolean setParameters(String[] names, long[] values, int page) {
	return setParameters(getParameterIndexes(names), values, page);
    }

    /**
     * Set the values of the parameters at the specified page.
     * 
     * @param parameterIndexes  indexes of an existing parameters
     * @param values            array of values to be set
     * @param page              page to set values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setParameters(int[] parameterIndexes, long[] values, int page) {
	int n, i;
	n = Array.getLength(parameterIndexes);
	if (n != Array.getLength(values)) {
	    setError("Unequal number of array elements (setParameters)");
	    return false;
	}
	for ( i=0 ; i < n ; i++ )
	    if (!setParameter(parameterIndexes[i],values[i],page))
		return false;
	return true;
    }

    /**
     * Set the values of the parameters at the specified page.
     * 
     * @param names            names of an existing parameters
     * @param values           array of values to be set
     * @param page             page to set values
     * @return                 <code>true</code> if successful
     *                         <code>false</code> if an error occurs
     */
    public boolean setParameters(String[] names, float[] values, int page) {
	return setParameters(getParameterIndexes(names), SDDSUtil.castArrayAsDouble(values), page);
    }

    /**
     * Set the values of the parameters at the specified page.
     * 
     * @param parameterIndexes  indexes of an existing parameters
     * @param values            array of values to be set
     * @param page              page to set values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setParameters(int[] parameterIndexes, float[] values, int page) {
	return setParameters(parameterIndexes, SDDSUtil.castArrayAsDouble(values), page);
    }

    /**
     * Set the values of the parameters at the specified page.
     * 
     * @param names            names of an existing parameters
     * @param values           array of values to be set
     * @param page             page to set values
     * @return                 <code>true</code> if successful
     *                         <code>false</code> if an error occurs
     */
    public boolean setParameters(String[] names, double[] values, int page) {
	return setParameters(getParameterIndexes(names), values, page);
    }

    /**
     * Set the values of the parameters at the specified page.
     * 
     * @param parameterIndexes  indexes of an existing parameters
     * @param values            array of values to be set
     * @param page              page to set values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setParameters(int[] parameterIndexes, double[] values, int page) {
	int n, i;
	n = Array.getLength(parameterIndexes);
	if (n != Array.getLength(values)) {
	    setError("Unequal number of array elements (setParameters)");
	    return false;
	}
	for ( i=0 ; i < n ; i++ )
	    if (!setParameter(parameterIndexes[i],values[i],page))
		return false;
	return true;
    }

    /**
     * Set the values of the parameters at the specified page.
     * 
     * @param names            names of an existing parameters
     * @param values           array of values to be set
     * @param page             page to set values
     * @return                 <code>true</code> if successful
     *                         <code>false</code> if an error occurs
     */
    public boolean setParameters(String[] names, String[] values, int page) {
	return setParameters(getParameterIndexes(names), values, page);
    }

    /**
     * Set the values of the parameters at the specified page.
     * 
     * @param parameterIndexes  indexes of an existing parameters
     * @param values            array of values to be set
     * @param page              page to set values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setParameters(int[] parameterIndexes, String[] values, int page) {
	int n, i;
	n = Array.getLength(parameterIndexes);
	if (n != Array.getLength(values)) {
	    setError("Unequal number of array elements (setParameters)");
	    return false;
	}
	for ( i=0 ; i < n ; i++ )
	    if (!setParameter(parameterIndexes[i],values[i],page))
		return false;
	return true;
    }

    /**
     * Set the values of the parameters at the specified page.
     * 
     * @param names            names of an existing parameters
     * @param values           array of values to be set
     * @param page             page to set values
     * @return                 <code>true</code> if successful
     *                         <code>false</code> if an error occurs
     */
    public boolean setParameters(String[] names, char[] values, int page) {
	return setParameters(getParameterIndexes(names), SDDSUtil.castArrayAsString(values), page);
    }

    /**
     * Set the values of the parameters at the specified page.
     * 
     * @param parameterIndexes  indexes of an existing parameters
     * @param values            array of values to be set
     * @param page              page to set values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setParameters(int[] parameterIndexes, char[] values, int page) {
	return setParameters(parameterIndexes, SDDSUtil.castArrayAsString(values), page);
    }




    /**
     * Set the values of the parameters starting at the specified page.
     * 
     * @param names            names of an existing parameters
     * @param values           two dimensional array of values to be set
     * @param startPage        page to start setting the values
     * @return                 <code>true</code> if successful
     *                         <code>false</code> if an error occurs
     */
    public boolean setParameters(String[] names, short[][] values, int startPage) {
	return setParameters(getParameterIndexes(names), values, startPage);
    }

    /**
     * Set the values of the parameters starting at the specified page.
     * 
     * @param parameterIndexes  indexes of an existing parameters
     * @param values            two dimensional array of values to be set
     * @param startPage         page to start setting the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setParameters(int[] parameterIndexes, short[][] values, int startPage) {
	int n, i;
	n = Array.getLength(parameterIndexes);
	if (n != Array.getLength(values)) {
	    setError("Unequal number of array elements (setParameters)");
	    return false;
	}
	for ( i=0 ; i < n ; i++ )
	    if (!setParameter(parameterIndexes[i],SDDSUtil.castArrayAsLong(values[i]),startPage))
		return false;
	return true;
    }

    /**
     * Set the values of the parameters starting at the specified page.
     * 
     * @param names            names of an existing parameters
     * @param values           two dimensional array of values to be set
     * @param startPage        page to start setting the values
     * @return                 <code>true</code> if successful
     *                         <code>false</code> if an error occurs
     */
    public boolean setParameters(String[] names, int[][] values, int startPage) {
	return setParameters(getParameterIndexes(names), values, startPage);
    }

    /**
     * Set the values of the parameters starting at the specified page.
     * 
     * @param parameterIndexes  indexes of an existing parameters
     * @param values            two dimensional array of values to be set
     * @param startPage         page to start setting the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setParameters(int[] parameterIndexes, int[][] values, int startPage) {
	int n, i;
	n = Array.getLength(parameterIndexes);
	if (n != Array.getLength(values)) {
	    setError("Unequal number of array elements (setParameters)");
	    return false;
	}
	for ( i=0 ; i < n ; i++ )
	    if (!setParameter(parameterIndexes[i],SDDSUtil.castArrayAsLong(values[i]),startPage))
		return false;
	return true;
    }

    /**
     * Set the values of the parameters starting at the specified page.
     * 
     * @param names            names of an existing parameters
     * @param values           two dimensional array of values to be set
     * @param startPage        page to start setting the values
     * @return                 <code>true</code> if successful
     *                         <code>false</code> if an error occurs
     */
    public boolean setParameters(String[] names, long[][] values, int startPage) {
	return setParameters(getParameterIndexes(names), values, startPage);
    }

    /**
     * Set the values of the parameters starting at the specified page.
     * 
     * @param parameterIndexes  indexes of an existing parameters
     * @param values            two dimensional array of values to be set
     * @param startPage         page to start setting the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setParameters(int[] parameterIndexes, long[][] values, int startPage) {
	int n, i;
	n = Array.getLength(parameterIndexes);
	if (n != Array.getLength(values)) {
	    setError("Unequal number of array elements (setParameters)");
	    return false;
	}
	for ( i=0 ; i < n ; i++ )
	    if (!setParameter(parameterIndexes[i],values[i],startPage))
		return false;
	return true;
    }

    /**
     * Set the values of the parameters starting at the specified page.
     * 
     * @param names            names of an existing parameters
     * @param values           two dimensional array of values to be set
     * @param startPage        page to start setting the values
     * @return                 <code>true</code> if successful
     *                         <code>false</code> if an error occurs
     */
    public boolean setParameters(String[] names, float[][] values, int startPage) {
	return setParameters(getParameterIndexes(names), values, startPage);
    }

    /**
     * Set the values of the parameters starting at the specified page.
     * 
     * @param parameterIndexes  indexes of an existing parameters
     * @param values            two dimensional array of values to be set
     * @param startPage         page to start setting the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setParameters(int[] parameterIndexes, float[][] values, int startPage) {
	int n, i;
	n = Array.getLength(parameterIndexes);
	if (n != Array.getLength(values)) {
	    setError("Unequal number of array elements (setParameters)");
	    return false;
	}
	for ( i=0 ; i < n ; i++ )
	    if (!setParameter(parameterIndexes[i],SDDSUtil.castArrayAsDouble(values[i]),startPage))
		return false;
	return true;
    }

    /**
     * Set the values of the parameters starting at the specified page.
     * 
     * @param names            names of an existing parameters
     * @param values           two dimensional array of values to be set
     * @param startPage        page to start setting the values
     * @return                 <code>true</code> if successful
     *                         <code>false</code> if an error occurs
     */
    public boolean setParameters(String[] names, double[][] values, int startPage) {
	return setParameters(getParameterIndexes(names), values, startPage);
    }

    /**
     * Set the values of the parameters starting at the specified page.
     * 
     * @param parameterIndexes  indexes of an existing parameters
     * @param values            two dimensional array of values to be set
     * @param startPage         page to start setting the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setParameters(int[] parameterIndexes, double[][] values, int startPage) {
	int n, i;
	n = Array.getLength(parameterIndexes);
	if (n != Array.getLength(values)) {
	    setError("Unequal number of array elements (setParameters)");
	    return false;
	}
	for ( i=0 ; i < n ; i++ )
	    if (!setParameter(parameterIndexes[i],values[i],startPage))
		return false;
	return true;
    }

    /**
     * Set the values of the parameters starting at the specified page.
     * 
     * @param names            names of an existing parameters
     * @param values           two dimensional array of values to be set
     * @param startPage        page to start setting the values
     * @return                 <code>true</code> if successful
     *                         <code>false</code> if an error occurs
     */
    public boolean setParameters(String[] names, String[][] values, int startPage) {
	return setParameters(getParameterIndexes(names), values, startPage);
    }

    /**
     * Set the values of the parameters starting at the specified page.
     * 
     * @param parameterIndexes  indexes of an existing parameters
     * @param values            two dimensional array of values to be set
     * @param startPage         page to start setting the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setParameters(int[] parameterIndexes, String[][] values, int startPage) {
	int n, i;
	n = Array.getLength(parameterIndexes);
	if (n != Array.getLength(values)) {
	    setError("Unequal number of array elements (setParameters)");
	    return false;
	}
	for ( i=0 ; i < n ; i++ )
	    if (!setParameter(parameterIndexes[i],values[i],startPage))
		return false;
	return true;
    }

    /**
     * Set the values of the parameters starting at the specified page.
     * 
     * @param names            names of an existing parameters
     * @param values           two dimensional array of values to be set
     * @param startPage        page to start setting the values
     * @return                 <code>true</code> if successful
     *                         <code>false</code> if an error occurs
     */
    public boolean setParameters(String[] names, char[][] values, int startPage) {
	return setParameters(getParameterIndexes(names), values, startPage);
    }

    /**
     * Set the values of the parameters starting at the specified page.
     * 
     * @param parameterIndexes  indexes of an existing parameters
     * @param values            two dimensional array of values to be set
     * @param startPage         page to start setting the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setParameters(int[] parameterIndexes, char[][] values, int startPage) {
	int n, i;
	n = Array.getLength(parameterIndexes);
	if (n != Array.getLength(values)) {
	    setError("Unequal number of array elements (setParameters)");
	    return false;
	}
	for ( i=0 ; i < n ; i++ )
	    if (!setParameter(parameterIndexes[i],SDDSUtil.castArrayAsString(values[i]),startPage))
		return false;
	return true;
    }

    /**
     * Set the values of the array at the specified page.
     * 
     * @param name              name of an existing array
     * @param values            array of values to be set
     * @param page              page to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setArray(String name, short[] values, int page) {
	return setArray(getArrayIndex(name), SDDSUtil.castArrayAsLong(values),page);
    }

    /**
     * Set the values of the array at the specified page.
     * 
     * @param arrayIndex        index of an existing array
     * @param values            array of values to be set
     * @param page              page to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setArray(int arrayIndex, short[] values, int page) {
	if ((arrayIndex < 0) || (arrayIndex >= arrayCount)){
	    setError("array index " + arrayIndex + " does not exist (setArray)");
	    return false;
	}
	arrayObject[arrayIndex].setValues(SDDSUtil.castArrayAsLong(values), page);
	return true;
    }

    /**
     * Set the values of the array at the specified page.
     * 
     * @param name              name of an existing array
     * @param values            array of values to be set
     * @param page              page to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setArray(String name, int[] values, int page) {
	return setArray(getArrayIndex(name), SDDSUtil.castArrayAsLong(values),page);
    }

    /**
     * Set the values of the array at the specified page.
     * 
     * @param arrayIndex        index of an existing array
     * @param values            array of values to be set
     * @param page              page to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setArray(int arrayIndex, int[] values, int page) {
	if ((arrayIndex < 0) || (arrayIndex >= arrayCount)){
	    setError("array index " + arrayIndex + " does not exist (setArray)");
	    return false;
	}
	arrayObject[arrayIndex].setValues(SDDSUtil.castArrayAsLong(values), page);
	return true;
    }

    /**
     * Set the values of the array at the specified page.
     * 
     * @param name              name of an existing array
     * @param values            array of values to be set
     * @param page              page to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setArray(String name, long[] values, int page) {
	return setArray(getArrayIndex(name), values,page);
    }

    /**
     * Set the values of the array at the specified page.
     * 
     * @param arrayIndex        index of an existing array
     * @param values            array of values to be set
     * @param page              page to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setArray(int arrayIndex, long[] values, int page) {
	if ((arrayIndex < 0) || (arrayIndex >= arrayCount)){
	    setError("array index " + arrayIndex + " does not exist (setArray)");
	    return false;
	}
	arrayObject[arrayIndex].setValues(values, page);
	return true;
    }

    /**
     * Set the values of the array at the specified page.
     * 
     * @param name              name of an existing array
     * @param values            array of values to be set
     * @param page              page to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setArray(String name, float[] values, int page) {
	return setArray(getArrayIndex(name), SDDSUtil.castArrayAsDouble(values),page);
    }

    /**
     * Set the values of the array at the specified page.
     * 
     * @param arrayIndex        index of an existing array
     * @param values            array of values to be set
     * @param page              page to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setArray(int arrayIndex, float[] values, int page) {
	if ((arrayIndex < 0) || (arrayIndex >= arrayCount)){
	    setError("array index " + arrayIndex + " does not exist (setArray)");
	    return false;
	}
	arrayObject[arrayIndex].setValues(SDDSUtil.castArrayAsDouble(values), page);
	return true;
    }

    /**
     * Set the values of the array at the specified page.
     * 
     * @param name              name of an existing array
     * @param values            array of values to be set
     * @param page              page to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setArray(String name, double[] values, int page) {
	return setArray(getArrayIndex(name), values,page);
    }

    /**
     * Set the values of the array at the specified page.
     * 
     * @param arrayIndex        index of an existing array
     * @param values            array of values to be set
     * @param page              page to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setArray(int arrayIndex, double[] values, int page) {
	if ((arrayIndex < 0) || (arrayIndex >= arrayCount)){
	    setError("array index " + arrayIndex + " does not exist (setArray)");
	    return false;
	}
	arrayObject[arrayIndex].setValues(values, page);
	return true;
    }

    /**
     * Set the values of the array at the specified page.
     * 
     * @param name              name of an existing array
     * @param values            array of values to be set
     * @param page              page to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setArray(String name, String[] values, int page) {
	return setArray(getArrayIndex(name), values,page);
    }

    /**
     * Set the values of the array at the specified page.
     * 
     * @param arrayIndex        index of an existing array
     * @param values            array of values to be set
     * @param page              page to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setArray(int arrayIndex, String[] values, int page) {
	if ((arrayIndex < 0) || (arrayIndex >= arrayCount)){
	    setError("array index " + arrayIndex + " does not exist (setArray)");
	    return false;
	}
	arrayObject[arrayIndex].setValues(values, page);
	return true;
    }

    /**
     * Set the values of the array at the specified page.
     * 
     * @param name              name of an existing array
     * @param values            array of values to be set
     * @param page              page to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setArray(String name, char[] values, int page) {
	return setArray(getArrayIndex(name), values,page);
    }

    /**
     * Set the values of the array at the specified page.
     * 
     * @param arrayIndex        index of an existing array
     * @param values            array of values to be set
     * @param page              page to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setArray(int arrayIndex, char[] values, int page) {
	if ((arrayIndex < 0) || (arrayIndex >= arrayCount)){
	    setError("array index " + arrayIndex + " does not exist (setArray)");
	    return false;
	}
	arrayObject[arrayIndex].setValues(SDDSUtil.castArrayAsString(values), page);
	return true;
    }

    /**
     * Set the values of the array at the specified page.
     * 
     * @param name              name of an existing array
     * @param values            array of values to be set
     * @param page              page to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setArray(String name, Object[] values, int page) {
	return setArray(getArrayIndex(name), values, page);
    }

    /**
     * Set the values of the array at the specified page.
     * 
     * @param arrayIndex        index of an existing array
     * @param values            array of values to be set
     * @param page              page to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setArray(int arrayIndex, Object[] values, int page) {
	int type;
	if ((arrayIndex < 0) || (arrayIndex >= arrayCount)){
	    setError("array index " + arrayIndex + " does not exist (setArray)");
	    return false;
	}
	type = getArrayType(arrayIndex);
	switch (type) {
	case SDDS_SHORT:
	case SDDS_LONG:
	case SDDS_FLOAT:
	case SDDS_DOUBLE:
	    arrayObject[arrayIndex].setValues(SDDSUtil.castArrayAsDouble(values, type), page);
	    return true;
	case SDDS_STRING:
	case SDDS_CHARACTER:
	    arrayObject[arrayIndex].setValues(SDDSUtil.castArrayAsString(values, type), page);
	    return true;
	}
	return false;
    }
    
    /**
     * Set the values of the column at the specified page.
     * 
     * @param name              name of an existing column
     * @param values            array of values to be set
     * @param page              page to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumn(String name, short[] values, int page) {
	return setColumn(getColumnIndex(name), SDDSUtil.castArrayAsLong(values),page,1);
    }

    /**
     * Set the values of the column at the specified page starting at the specified row.
     * 
     * @param name              name of an existing column
     * @param values            array of values to be set
     * @param page              page to set the values
     * @param startRow          row to start setting the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumn(String name, short[] values, int page, int startRow) {
	return setColumn(getColumnIndex(name), SDDSUtil.castArrayAsLong(values),page,startRow);
    }

    /**
     * Set the values of the column at the specified page.
     * 
     * @param columnIndex       index of an existing column
     * @param values            array of values to be set
     * @param page              page to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumn(int columnIndex, short[] values, int page) {
	return setColumn(columnIndex, SDDSUtil.castArrayAsLong(values), page, 1);
    }

    /**
     * Set the values of the column at the specified page starting at the specified row.
     * 
     * @param columnIndex       index of an existing column
     * @param values            array of values to be set
     * @param page              page to set the values
     * @param startRow          row to start setting the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumn(int columnIndex, short[] values, int page, int startRow) {
	return setColumn(columnIndex, SDDSUtil.castArrayAsLong(values), page, startRow);
    }

    /**
     * Set the values of the column at the specified page.
     * 
     * @param name              name of an existing column
     * @param values            array of values to be set
     * @param page              page to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumn(String name, int[] values, int page) {
	return setColumn(getColumnIndex(name), SDDSUtil.castArrayAsLong(values),page,1);
    }

    /**
     * Set the values of the column at the specified page starting at the specified row.
     * 
     * @param name              name of an existing column
     * @param values            array of values to be set
     * @param page              page to set the values
     * @param startRow          row to start setting the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumn(String name, int[] values, int page, int startRow) {
	return setColumn(getColumnIndex(name), SDDSUtil.castArrayAsLong(values),page,startRow);
    }

    /**
     * Set the values of the column at the specified page.
     * 
     * @param columnIndex       index of an existing column
     * @param values            array of values to be set
     * @param page              page to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumn(int columnIndex, int[] values, int page) {
	return setColumn(columnIndex, SDDSUtil.castArrayAsLong(values), page, 1);
    }

    /**
     * Set the values of the column at the specified page starting at the specified row.
     * 
     * @param columnIndex       index of an existing column
     * @param values            array of values to be set
     * @param page              page to set the values
     * @param startRow          row to start setting the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumn(int columnIndex, int[] values, int page, int startRow) {
	return setColumn(columnIndex, SDDSUtil.castArrayAsLong(values), page, startRow);
    }

    /**
     * Set the values of the column at the specified page.
     * 
     * @param name              name of an existing column
     * @param values            array of values to be set
     * @param page              page to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumn(String name, long[] values, int page) {
	return setColumn(getColumnIndex(name),values,page,1);
    }

    /**
     * Set the values of the column at the specified page starting at the specified row.
     * 
     * @param name              name of an existing column
     * @param values            array of values to be set
     * @param page              page to set the values
     * @param startRow          row to start setting the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumn(String name, long[] values, int page, int startRow) {
	return setColumn(getColumnIndex(name),values,page,startRow);
    }

    /**
     * Set the values of the column at the specified page.
     * 
     * @param columnIndex       index of an existing column
     * @param values            array of values to be set
     * @param page              page to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumn(int columnIndex, long[] values, int page) {
	return setColumn(columnIndex,values,page,1);
    }

    /**
     * Set the values of the column at the specified page starting at the specified row.
     * 
     * @param columnIndex       index of an existing column
     * @param values            array of values to be set
     * @param page              page to set the values
     * @param startRow          row to start setting the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumn(int columnIndex, long[] values, int page, int startRow) {
	if ((columnIndex < 0) || (columnIndex >= columnCount)) {
	    setError("column index " + columnIndex + " does not exist (setColumn)");
	    return false;
	}
	columnObject[columnIndex].setValues(values, page, startRow);
	return true;
    }

    /**
     * Set the values of the column at the specified page.
     * 
     * @param name              name of an existing column
     * @param values            array of values to be set
     * @param page              page to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumn(String name, float[] values, int page) {
	return setColumn(getColumnIndex(name), SDDSUtil.castArrayAsDouble(values),page,1);
    }

    /**
     * Set the values of the column at the specified page starting at the specified row.
     * 
     * @param name              name of an existing column
     * @param values            array of values to be set
     * @param page              page to set the values
     * @param startRow          row to start setting the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumn(String name, float[] values, int page, int startRow) {
	return setColumn(getColumnIndex(name), SDDSUtil.castArrayAsDouble(values),page,startRow);
    }

    /**
     * Set the values of the column at the specified page.
     * 
     * @param columnIndex       index of an existing column
     * @param values            array of values to be set
     * @param page              page to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumn(int columnIndex, float[] values, int page) {
	return setColumn(columnIndex, SDDSUtil.castArrayAsDouble(values), page, 1);
    }

    /**
     * Set the values of the column at the specified page starting at the specified row.
     * 
     * @param columnIndex       index of an existing column
     * @param values            array of values to be set
     * @param page              page to set the values
     * @param startRow          row to start setting the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumn(int columnIndex, float[] values, int page, int startRow) {
	return setColumn(columnIndex, SDDSUtil.castArrayAsDouble(values), page, startRow);
    }

    /**
     * Set the values of the column at the specified page.
     * 
     * @param name              name of an existing column
     * @param values            array of values to be set
     * @param page              page to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumn(String name, double[] values, int page) {
	return setColumn(getColumnIndex(name),values,page,1);
    }

    /**
     * Set the values of the column at the specified page starting at the specified row.
     * 
     * @param name              name of an existing column
     * @param values            array of values to be set
     * @param page              page to set the values
     * @param startRow          row to start setting the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumn(String name, double[] values, int page, int startRow) {
	return setColumn(getColumnIndex(name),values,page,startRow);
    }

    /**
     * Set the values of the column at the specified page.
     * 
     * @param columnIndex       index of an existing column
     * @param values            array of values to be set
     * @param page              page to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumn(int columnIndex, double[] values, int page) {
	return setColumn(columnIndex,values,page,1);
    }

    /**
     * Set the values of the column at the specified page starting at the specified row.
     * 
     * @param columnIndex       index of an existing column
     * @param values            array of values to be set
     * @param page              page to set the values
     * @param startRow          row to start setting the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumn(int columnIndex, double[] values, int page, int startRow) {
	if ((columnIndex < 0) || (columnIndex >= columnCount)) {
	    setError("column index " + columnIndex + " does not exist (setColumn)");
	    return false;
	}
	columnObject[columnIndex].setValues(values, page, startRow);
	return true;
    }

    /**
     * Set the values of the column at the specified page.
     * 
     * @param name              name of an existing column
     * @param values            array of values to be set
     * @param page              page to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumn(String name, String[] values, int page) {
	return setColumn(getColumnIndex(name),values,page,1);
    }

    /**
     * Set the values of the column at the specified page starting at the specified row.
     * 
     * @param name              name of an existing column
     * @param values            array of values to be set
     * @param page              page to set the values
     * @param startRow          row to start setting the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumn(String name, String[] values, int page, int startRow) {
	return setColumn(getColumnIndex(name),values,page,startRow);
    }

    /**
     * Set the values of the column at the specified page.
     * 
     * @param columnIndex       index of an existing column
     * @param values            array of values to be set
     * @param page              page to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumn(int columnIndex, String[] values, int page) {
	return setColumn(columnIndex,values,page,1);
    }

    /**
     * Set the values of the column at the specified page starting at the specified row.
     * 
     * @param columnIndex       index of an existing column
     * @param values            array of values to be set
     * @param page              page to set the values
     * @param startRow          row to start setting the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumn(int columnIndex, String[] values, int page, int startRow) {
	if ((columnIndex < 0) || (columnIndex >= columnCount)) {
	    setError("column index " + columnIndex + " does not exist (setColumn)");
	    return false;
	}
	columnObject[columnIndex].setValues(values, page, startRow);
	return true;
    }

    /**
     * Set the values of the column at the specified page.
     * 
     * @param name              name of an existing column
     * @param values            array of values to be set
     * @param page              page to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumn(String name, char[] values, int page) {
	return setColumn(getColumnIndex(name), SDDSUtil.castArrayAsString(values),page,1);
    }

    /**
     * Set the values of the column at the specified page starting at the specified row.
     * 
     * @param name              name of an existing column
     * @param values            array of values to be set
     * @param page              page to set the values
     * @param startRow          row to start setting the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumn(String name, char[] values, int page, int startRow) {
	return setColumn(getColumnIndex(name), SDDSUtil.castArrayAsString(values),page,startRow);
    }

    /**
     * Set the values of the column at the specified page.
     * 
     * @param columnIndex       index of an existing column
     * @param values            array of values to be set
     * @param page              page to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumn(int columnIndex, char[] values, int page) {
	return setColumn(columnIndex, SDDSUtil.castArrayAsString(values), page, 1);
    }

    /**
     * Set the values of the column at the specified page starting at the specified row.
     * 
     * @param columnIndex       index of an existing column
     * @param values            array of values to be set
     * @param page              page to set the values
     * @param startRow          row to start setting the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumn(int columnIndex, char[] values, int page, int startRow) {
	return setColumn(columnIndex, SDDSUtil.castArrayAsString(values), page, startRow);
    }

    /**
     * Set the values of the column at the specified page.
     * 
     * @param name              name of an existing column
     * @param values            array of values to be set
     * @param page              page to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumn(String name, Object[] values, int page) {
	return setColumn(getColumnIndex(name), values, page, 1);
    }

    /**
     * Set the values of the column at the specified page starting at the specified row.
     * 
     * @param name              name of an existing column
     * @param values            array of values to be set
     * @param page              page to set the values
     * @param startRow          row to start setting the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumn(String name, Object[] values, int page, int startRow) {
	return setColumn(getColumnIndex(name), values, page, startRow);
    }

    /**
     * Set the values of the column at the specified page.
     * 
     * @param columnIndex       index of an existing column
     * @param values            array of values to be set
     * @param page              page to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumn(int columnIndex, Object[] values, int page) {
	return setColumn(columnIndex, values, page, 1);
    }

    /**
     * Set the values of the column at the specified page starting at the specified row.
     * 
     * @param columnIndex       index of an existing column
     * @param values            array of values to be set
     * @param page              page to set the values
     * @param startRow          row to start setting the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumn(int columnIndex, Object[] values, int page, int startRow) {
	int type;
	type = getColumnType(columnIndex);
	switch (type) {
	case SDDS_SHORT:
	case SDDS_LONG:
	case SDDS_FLOAT:
	case SDDS_DOUBLE:
	    return setColumn(columnIndex, SDDSUtil.castArrayAsDouble(values, type), page, startRow);
	case SDDS_STRING:
	case SDDS_CHARACTER:
	    return setColumn(columnIndex, SDDSUtil.castArrayAsString(values, type), page, startRow);
	}
	return false;
    }

    /**
     * Set the value of the column element at the specified page and row.
     * 
     * @param name              name of an existing column
     * @param value             value to be set
     * @param page              page to set the values
     * @param row               row to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumnElement(String name, short value, int page, int row) {
	long[] v = new long[1];
	v[0] = (long)value;
	return setColumn(getColumnIndex(name), v ,page,row);
    }

    /**
     * Set the value of the column element at the specified page and row.
     * 
     * @param columnIndex       index of an existing column
     * @param value             value to be set
     * @param page              page to set the values
     * @param row               row to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumnElement(int columnIndex, short value, int page, int row) {
	long[] v = new long[1];
	v[0] = (long)value;
	return setColumn(columnIndex, v ,page,row);
    }

    /**
     * Set the value of the column element at the specified page and row.
     * 
     * @param name              name of an existing column
     * @param value             value to be set
     * @param page              page to set the values
     * @param row               row to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumnElement(String name, int value, int page, int row) {
	long[] v = new long[1];
	v[0] = (long)value;
	return setColumn(getColumnIndex(name), v ,page,row);
    }

    /**
     * Set the value of the column element at the specified page and row.
     * 
     * @param columnIndex        index of an existing column
     * @param value             value to be set
     * @param page              page to set the values
     * @param row               row to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumnElement(int columnIndex, int value, int page, int row) {
	long[] v = new long[1];
	v[0] = (long)value;
	return setColumn(columnIndex, v ,page,row);
    }

    /**
     * Set the value of the column element at the specified page and row.
     * 
     * @param name              name of an existing column
     * @param value             value to be set
     * @param page              page to set the values
     * @param row               row to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumnElement(String name, long value, int page, int row) {
	long[] v = new long[1];
	v[0] = value;
	return setColumn(getColumnIndex(name), v ,page,row);
    }

    /**
     * Set the value of the column element at the specified page and row.
     * 
     * @param columnIndex        index of an existing column
     * @param value             value to be set
     * @param page              page to set the values
     * @param row               row to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumnElement(int columnIndex, long value, int page, int row) {
	long[] v = new long[1];
	v[0] = value;
	return setColumn(columnIndex, v ,page,row);
    }

    /**
     * Set the value of the column element at the specified page and row.
     * 
     * @param name              name of an existing column
     * @param value             value to be set
     * @param page              page to set the values
     * @param row               row to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumnElement(String name, float value, int page, int row) {
	double[] v = new double[1];
	v[0] = (double)value;
	return setColumn(getColumnIndex(name), v ,page,row);
    }

    /**
     * Set the value of the column element at the specified page and row.
     * 
     * @param columnIndex        index of an existing column
     * @param value             value to be set
     * @param page              page to set the values
     * @param row               row to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumnElement(int columnIndex, float value, int page, int row) {
	double[] v = new double[1];
	v[0] = (double)value;
	return setColumn(columnIndex, v ,page,row);
    }

    /**
     * Set the value of the column element at the specified page and row.
     * 
     * @param name              name of an existing column
     * @param value             value to be set
     * @param page              page to set the values
     * @param row               row to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumnElement(String name, double value, int page, int row) {
	double[] v = new double[1];
	v[0] = value;
	return setColumn(getColumnIndex(name), v ,page,row);
    }

    /**
     * Set the value of the column element at the specified page and row.
     * 
     * @param columnIndex        index of an existing column
     * @param value             value to be set
     * @param page              page to set the values
     * @param row               row to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumnElement(int columnIndex, double value, int page, int row) {
	double[] v = new double[1];
	v[0] = value;
	return setColumn(columnIndex, v ,page,row);
    }

    /**
     * Set the value of the column element at the specified page and row.
     * 
     * @param name              name of an existing column
     * @param value             value to be set
     * @param page              page to set the values
     * @param row               row to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumnElement(String name, String value, int page, int row) {
	String[] v = new String[1];
	v[0] = value;
	return setColumn(getColumnIndex(name), v ,page,row);
    }

    /**
     * Set the value of the column element at the specified page and row.
     * 
     * @param columnIndex        index of an existing column
     * @param value             value to be set
     * @param page              page to set the values
     * @param row               row to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumnElement(int columnIndex, String value, int page, int row) {
	String[] v = new String[1];
	v[0] = value;
	return setColumn(columnIndex, v ,page,row);
    }

    /**
     * Set the value of the column element at the specified page and row.
     * 
     * @param name              name of an existing column
     * @param value             value to be set
     * @param page              page to set the values
     * @param row               row to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumnElement(String name, char value, int page, int row) {
	char[] v = new char[1];
	v[0] = value;
	return setColumn(getColumnIndex(name), v ,page,row);
    }

    /**
     * Set the value of the column element at the specified page and row.
     * 
     * @param columnIndex        index of an existing column
     * @param value             value to be set
     * @param page              page to set the values
     * @param row               row to set the values
     * @return                  <code>true</code> if successful
     *                          <code>false</code> if an error occurs
     */
    public boolean setColumnElement(int columnIndex, char value, int page, int row) {
	char[] v = new char[1];
	v[0] = value;
	return setColumn(columnIndex, v ,page,row);
    }

    /**
     * Write the layout to a file.
     *
     * @return            <code>true</code> if the layout was successfully written<br>
     *                    <code>false</code> if an error occured
     */
    public boolean writeLayout() {
	String endian;
	try {
	    dos.writeBytes("SDDS1\n");
	    if (!asciiFile)
		if (bigEndian)
		    dos.writeBytes("!# big-endian\n");
		else
		    dos.writeBytes("!# little-endian\n");
	    if (!writeDescription())
		return false;
	    if (!writeParameterHeaders())
		return false;
	    if (!writeArrayHeaders())
		return false;
	    if (!writeColumnHeaders())
		return false;
	    if (asciiFile) {
		if (noRowCount)
		    dos.writeBytes("&data mode=ascii, no_row_counts=1, &end\n");
		else
		    dos.writeBytes("&data mode=ascii, &end\n");
	    } else {
		/* This will be used in the future
		if (bigEndian) {
		    dos.writeBytes("&data mode=binary, endian=big, &end\n");
		} else {
		    dos.writeBytes("&data mode=binary, endian=little, &end\n");
		}
		*/
		dos.writeBytes("&data mode=binary, &end\n");
	    }
	    dos.flush();
	} catch (Exception e) {
	    setError("unable to write layout (writeLayout)");
	    return false;
	}
	return true;
    }

    /**
     * Write the description to a file.
     *
     * @return            <code>true</code> if the description was successfully written<br>
     *                    <code>false</code> if an error occured
     */
    public boolean writeDescription() {
        String line;
	if ((descriptionText == null) && (descriptionContents == null))
	    return true;
	line = "&description ";
	if (descriptionText != null) {
	    line = line.concat("text=" + SDDSUtil.prepareString(descriptionText) + ", ");
	}
	if (descriptionContents != null) {
	    line = line.concat("contents=" + SDDSUtil.prepareString(descriptionContents) + ", ");
	}
	line = line.concat("&end\n");
	try {
	    dos.writeBytes(line);
	} catch (Exception e) {
	    setError("unable to write description (writeDescription)");
	    return false;
	}
	return true;
    }

    /**
     * Write the parameter headers to a file.
     *
     * @return            <code>true</code> if the headers were successfully written<br>
     *                    <code>false</code> if an error occured
     */
    public boolean writeParameterHeaders() {
	int i;
	for ( i=0 ; i<parameterCount ; i++ ) {
	    if (!writeParameterHeader(i))
		return false;
	}
	return true;
    }

    /**
     * Write the specified parameter header to a file.
     * 
     * @param index      index of an existing parameter
     * @return           <code>true</code> if the header was successfully written<br>
     *                   <code>false</code> if an error occured
     */
    public boolean writeParameterHeader(int index) {
	if ((index < 0) || (index >= parameterCount)) {
	    setError("invalid parameter index (writeParameterHeader)");
	    return false;
	}
	if (parameterObject[index].writeHeader(dos))
	    return true;
	setError("unable to write parameter header (writeParameterHeader)");
	return false;
    }

    /**
     * Write the array headers to a file.
     *
     * @return            <code>true</code> if the headers were successfully written<br>
     *                    <code>false</code> if an error occurred
     */
    public boolean writeArrayHeaders()
    {
	int i;
	for ( i=0; i<arrayCount;i++){
	    if (!writeArrayHeader(i))
		return false;
	}
	return true;
    }

    
    /**
     * Write the specified array header to a file.
     * 
     * @param index      index of an existing array
     * @return           <code>true</code> if the header was successfully written<br>
     *                   <code>false</code> if an error occured
     */
    public boolean writeArrayHeader(int index) {
	String line;
	if ((index < 0) || (index >= arrayCount)) {
	    setError("invalid array index (writeArrayHeader)");
	    return false;
	}
	if (arrayObject[index].writeHeader(dos))
	    return true;
	setError("unable to write array header (writeColumnHeader)");
	return false;	
    }
    /**
     * Write the column headers to a file.
     *
     * @return            <code>true</code> if the headers were successfully written<br>
     *                    <code>false</code> if an error occured
     */
    public boolean writeColumnHeaders() {
	int i;
	for ( i=0 ; i<columnCount ; i++ ) {
	    if (!writeColumnHeader(i))
		return false;
	}
	return true;
    }

    /**
     * Write the specified column header to a file.
     * 
     * @param index      index of an existing column
     * @return           <code>true</code> if the header was successfully written<br>
     *                   <code>false</code> if an error occured
     */
    public boolean writeColumnHeader(int index) {
	String line;
	if ((index < 0) || (index >= columnCount)) {
	    setError("invalid column index (writeColumnHeader)");
	    return false;
	}
	if (columnObject[index].writeHeader(dos))
	    return true;
	setError("unable to write column header (writeColumnHeader)");
	return false;	
    }

    /**
     * Write all the pages to a file.
     * 
     * @return         <code>true</code> if successful<br>
     *                 <code>false</code> if an error occurs
     */
    public boolean writePages() {
	return writePages(1, pageCount());
    }

    /**
     * Get the number of pages in the SDDS object
     *
     * @return        number of pages
     */
    public int pageCount() {
	int endPage, i, pages;
	endPage = 0;
	for (i=0;i<parameterCount;i++) {
	    pages = parameterObject[i].getPageCount();
	    if (pages > endPage)
		 endPage = pages;
        }
	for (i=0;i<arrayCount;i++) {
	    pages = arrayObject[i].getPageCount();
	    if (pages > endPage)
		endPage = pages;
	}
	for (i=0;i<columnCount;i++) {
	    pages = columnObject[i].getPageCount();
	    if (pages > endPage)
		 endPage = pages;
        }
	return endPage;	
    }

    /**
     * Write specified the pages to a file.
     * 
     * @param startPage     first page to write
     * @param endPage       last page to write
     * @return         <code>true</code> if successful<br>
     *                 <code>false</code> if an error occurs
     */
    public boolean writePages(int startPage, int endPage) {
	int i;
	i = startPage;
	while (i <= endPage) {
	    if (!writePage(i))
		return false;
	    i++;
	}
	return true;
    }


    /**
     * Deletes the value of the column at a chosen page and at a chosen row
     *
     * @param page            the page of the column, must be greater then <code>0</code> 
     * @param row             the row of the column, must be greater then <code>0</code> 
     */
    public void deleteRow(int page, int row) {
	int i;
	for ( i=0 ; i<columnCount ; i++ ) {
	    columnObject[i].deleteRow(page, row);
	}
    }

    /**
     * Get the number of rows on a page.
     * 
     * @param page          page of sdds file
     * @return              number of rows on the specified page
     */
    public int getRowCount(int page) {
	int i, endRow, rows;
	endRow = 0;
	for ( i=0 ; i<columnCount ; i++ ) {
	    rows = columnObject[i].getRowCount(page);
	    if (rows > endRow)
		endRow = rows;
	}
	return endRow;
    }

    /**
     * Write specified the page to a file.
     * 
     * @param page          page to write
     * @return         <code>true</code> if successful<br>
     *                 <code>false</code> if an error occurs
     */
    public boolean writePage(int page) {
	int i, j, endRow, rows;
	endRow = 0;
	for ( i=0 ; i<columnCount ; i++ ) {
	    rows = columnObject[i].getRowCount(page);
	    if (rows > endRow)
		endRow = rows;
	}
	if (asciiFile) {
	    try {
		dos.writeBytes("!page number " + page + "\n");
	    } catch (IOException e) {
		setError("unable to write comment line to ascii page " + page + " (writePage)");
		return false;
	    }
	    for ( i=0 ; i<parameterCount ; i++ ) {
		if (!parameterObject[i].writeValue(dos, page, true)) {
		    setError("invalid page number (" + page + ") for parameter " + parameterObject[i].getName() + " (writePage)");
		    try {
			dos.flush();
		    } catch (IOException e) {
		    }
		    return false;
		}
	    }
	    if (arrayCount > 0) {
		for (i=0;i<arrayCount;i++){
		    if (!arrayObject[i].writeValues(dos, page, true)){
			setError("unable to write array data on page " + page + " (writePage)");
			return false;
		    }
		}
	    }
	    if (!noRowCount) {
		if (columnCount > 0) {
		    try {
			dos.writeBytes("\t" + endRow + "\n");
		    } catch (IOException e) {
			setError("unable to write number of rows to ascii page " + page + " (writePage)");
			return false;
		    }
		    if (endRow > 0) {
			for (j=1;j<=endRow;j++) {
			    for (i=0;i<columnCount;i++) {
				if (!columnObject[i].writeElement(dos, page, j, true)) {
				    setError("invalid column elemant at page (" + page + ") row (" + j + ") for column " + columnObject[i].getName() + " (writePage)");
				    try {
					dos.flush();
				    } catch (IOException e) {
				    }
				    return false;
				}
			    }
			    try {
				dos.writeBytes("\n");
			    } catch (IOException e) {
				setError("unable to write newline character to ascii page " + page + " (writePage)");
				return false;
			    }   
			}
		    }
		}
	    } else {
		if (endRow > 0) {
		    for (j=1;j<=endRow;j++) {
			for (i=0;i<columnCount;i++) {
			    if (!columnObject[i].writeElement(dos, page, j, true)) {
				setError("invalid column elemant at page (" + page + ") row (" + j + ") for column " + columnObject[i].getName() + " (writePage)");
				try {
				    dos.flush();
				} catch (IOException e) {
				}
				return false;
			    }
			}
			try {
			    dos.writeBytes("\n");
			} catch (IOException e) {
			    setError("unable to write newline character to ascii page " + page + " (writePage)");
			    return false;
			}
		    }
		}
		if (columnCount > 0) {
		    try {
			dos.writeBytes("\n");
		    } catch (IOException e) {
			setError("unable to write newline character to ascii page " + page + " (writePage)");
			return false;
		    }
		}
	    }
	    try {
		dos.flush();
	    } catch (IOException e) {
		setError("unable to flush data for ascii page " + page + " (writePage)");
		return false;
	    }
        } else {
	    if (bigEndian) {
		try {
		    dos.writeInt(endRow);
		} catch (IOException e) {
		    setError("unable to write row count to binary page " + page + " (writePage)");
		    return false;
		}
	        for ( i=0 ; i<parameterCount ; i++ ) {
	            if (!parameterObject[i].writeValue(dos, page, false)) {
		        setError("invalid page number (" + page + ") for parameter " + parameterObject[i].getName() + " (writePage)");
		        try {
                            dos.flush();
		        } catch (IOException e) {
		        }
		        return false;
	            }
	        }
		if (arrayCount > 0) {
		    for (i=0;i<arrayCount;i++){
			if (!arrayObject[i].writeValues(dos, page, false)){
			    setError("unable to write binary array data on page " + page + " (writePage)");
			    try {
				dos.flush();
			    }
			    catch (IOException e){
			    }
			    return false;
			}
		    }
		}
		for (j=1;j<=endRow;j++) {
		    for (i=0;i<columnCount;i++) {
			if (!columnObject[i].writeElement(dos, page, j, false)) {
			    setError("invalid column elemant at page (" + page + ") row (" + j + ") for column " + columnObject[i].getName() + " (writePage)");
			    try {
				dos.flush();
			    } catch (IOException e) {
			    }
			    return false;
			}
		    }
		}
	        try {
                    dos.flush();
	        } catch (IOException e) {
		    setError("unable to flush data for page " + page + " (writePage)");
		    return false;
	        }
            } else {
		try {
		    ledos.writeInt(endRow);
		} catch (IOException e) {
		    setError("unable to write row count to binary page " + page + " (writePage)");
		    return false;
		}
	        for ( i=0 ; i<parameterCount ; i++ ) {
	            if (!parameterObject[i].writeValue(ledos, page)) {
		        setError("invalid page number (" + page + ") for parameter " + parameterObject[i].getName() + " (writePage)");
		        try {
                            ledos.flush();
		        } catch (IOException e) {
		        }
		        return false;
	            }
	        }
		if (arrayCount > 0) {
		    for (i=0;i<arrayCount;i++){
			if (!arrayObject[i].writeValues(ledos, page)){
			    setError("unable to write array data on page " + page + " (writePage)");
			    try {
				ledos.flush();
			    }
			    catch (IOException e){
			    }
			    return false;
			}
		    }
		}
		for (j=1;j<=endRow;j++) {
		    for (i=0;i<columnCount;i++) {
			if (!columnObject[i].writeElement(ledos, page, j)) {
			    setError("invalid column elemant at page (" + page + ") row (" + j + ") for column " + columnObject[i].getName() + " (writePage)");
			    try {
				ledos.flush();
			    } catch (IOException e) {
			    }
			    return false;
			}
		    }
		}
	        try {
                    ledos.flush();
	        } catch (IOException e) {
		    setError("unable to flush data for page " + page + " (writePage)");
		    return false;
	        }
	    }
	}
	return true;
    }

    /**
     * add a single element to a column of data
     *
     * @param          column      Column number (first column is 1)
     * @param          v           value to add to the column
     *
     * @return         <code>true</code> if successful<br>
     *                 <code>false</code> if an error occurs
     */
    public boolean addColumnElement(int column, String v)
    {
	if (column < 1 || column > columnCount){
	    setError("Invalid column specified (addColumnElement)");
	    return false;
	}
	column--;
	String values[];
	int pcount = pageCount();
	int size = columnObject[column].getRowCount(pcount);
	if (size < 0){
	    size = 1;
	    values = new String[size];
	}
	else {
	    size++;
	    Object o[] =  (columnObject[column].getValues(pcount,false));
	    values = new String[size];
	    for (int i=0; i<size-1; i++)
		values[i] = o[i].toString();	    
	}
	
	values[size-1] = v;
	columnObject[column].setValues(values, pcount, 1);
	return true;
    }
    
    /**
     * add a single element to a column of data
     *
     * @param          name        Column name 
     * @param          v           value to add to the column
     *
     * @return         <code>true</code> if successful<br>
     *                 <code>false</code> if an error occurs
     */
    public boolean addColumnElement(String name, String v)
    {
	return addColumnElement(getColumnIndex(name)+1, v);	
    }
    
    /**
     * add a single element to a column of data
     *
     * @param          column      Column number (first column is 1)
     * @param          v           value to add to the column
     *
     * @return         <code>true</code> if successful<br>
     *                 <code>false</code> if an error occurs
     */
    public boolean addColumnElement(int column, char v)
    {
	Character c = new Character(v);
	String s = c.toString();
	return addColumnElement(column, s);
    }
    
    /**
     * add a single element to a column of data
     *
     * @param          name        Column name 
     * @param          v           value to add to the column
     *
     * @return         <code>true</code> if successful<br>
     *                 <code>false</code> if an error occurs
     */
    public boolean addColumnElement(String name, char v)
    {
	return addColumnElement(getColumnIndex(name)+1, v);	
    }
    
    /**
     * add a single element to a column of data
     *
     * @param          column      Column number (first column is 1)
     * @param          v           value to add to the column
     *
     * @return         <code>true</code> if successful<br>
     *                 <code>false</code> if an error occurs
     */
    public boolean addColumnElement(int column, double v)
    {
	if (column < 1 || column > columnCount){
	    setError("Invalid column specified (addColumnElement)");
	    return false;
	}
	column--;
	double values[];
	int pcount = pageCount();
	int size = columnObject[column].getRowCount(pcount);
	if (size < 0){
	    size = 1;
	    values = new double[size];
	}
	else {
	    size++;
	    Object o[] =  (columnObject[column].getValues(pcount,false));
	    values = new double[size];
	    for (int i=0; i<size-1; i++){
		try{
		    values[i] = ((Double)o[i]).doubleValue();
		}
		catch (Exception e){
		    setError("Attempt to set double value (" + v + ") for column " + column + " failed. (addColumnElement)");
		    return false;
		}
	    }
	}
	
	values[size-1] = v;
	columnObject[column].setValues(values, pcount, 1);
	return true;
    }
    
    /**
     * add a single element to a column of data
     *
     * @param          name        Column name 
     * @param          v           value to add to the column
     *
     * @return         <code>true</code> if successful<br>
     *                 <code>false</code> if an error occurs
     */
    public boolean addColumnElement(String name, double v)
    {
	return addColumnElement(getColumnIndex(name)+1, v);	
    }
    
    /**
     * add a single element to a column of data
     *
     * @param          column      Column number (first column is 1)
     * @param          v           value to add to the column
     *
     * @return         <code>true</code> if successful<br>
     *                 <code>false</code> if an error occurs
     */
    public boolean addColumnElement(int column, float v)
    {
	if (column < 1 || column > columnCount){
	    setError("Invalid column specified (addColumnElement)");
	    return false;
	}
	column--;
	double values[];
	int pcount = pageCount();
	int size = columnObject[column].getRowCount(pcount);
	if (size < 0){
	    size = 1;
	    values = new double[size];
	}
	else {
	    size++;
	    Object o[] =  (columnObject[column].getValues(pcount,false));
	    values = new double[size];
	    for (int i=0; i<size-1; i++){
		try{
		    values[i] = (double)((Float)o[i]).floatValue();
		}
		catch (Exception e){
		    setError("Attempt to set float value (" + v + ") for column " + column + " failed. (addColumnElement)");
		    return false;
		}
	    }
	}
	
	values[size-1] = (double)v;
	columnObject[column].setValues(values, pcount, 1);
	return true;
    }
    
    /**
     * add a single element to a column of data
     *
     * @param          name        Column name 
     * @param          v           value to add to the column
     *
     * @return         <code>true</code> if successful<br>
     *                 <code>false</code> if an error occurs
     */
    public boolean addColumnElement(String name, float v)
    {
	return addColumnElement(getColumnIndex(name)+1, v);	
    }
    
    /**
     * add a single element to a column of data
     *
     * @param          column      Column number (first column is 1)
     * @param          v           value to add to the column
     *
     * @return         <code>true</code> if successful<br>
     *                 <code>false</code> if an error occurs
     */
    public boolean addColumnElement(int column, long v)
    {
	if (column < 1 || column > columnCount){
	    setError("Invalid column specified (addColumnElement)");
	    return false;
	}
	column--;
	long values[];
	int pcount = pageCount();
	int size = columnObject[column].getRowCount(pcount);
	if (size < 0){
	    size = 1;
	    values = new long[size];
	}
	else {
	    size++;
	    Object o[] =  (columnObject[column].getValues(pcount,false));
	    values = new long[size];
	    for (int i=0; i<size-1; i++){
		try{
		    values[i] = ((Long)o[i]).longValue();
		}
		catch (Exception e){
		    setError("Attempt to set long value (" + v + ") for column " + column + " failed. (addColumnElement)");
		    return false;
		}
	    }
	}
	
	values[size-1] = v;
	columnObject[column].setValues(values, pcount, 1);
	return true;
    }
    
    /**
     * add a single element to a column of data
     *
     * @param          name        Column name 
     * @param          v           value to add to the column
     *
     * @return         <code>true</code> if successful<br>
     *                 <code>false</code> if an error occurs
     */
    public boolean addColumnElement(String name, long v)
    {
	return addColumnElement(getColumnIndex(name)+1, v);	
    }
    
    /**
     * add a single element to a column of data
     *
     * @param          column      Column number (first column is 1)
     * @param          v           value to add to the column
     *
     * @return         <code>true</code> if successful<br>
     *                 <code>false</code> if an error occurs
     */
    public boolean addColumnElement(int column, short v)
    {
	if (column < 1 || column > columnCount){
	    setError("Invalid column specified (addColumnElement)");
	    return false;
	}
	column--;
	long values[];
	int pcount = pageCount();
	int size = columnObject[column].getRowCount(pcount);
	if (size < 0){
	    size = 1;
	    values = new long[size];
	}
	else {
	    size++;
	    Object o[] =  (columnObject[column].getValues(pcount,false));
	    values = new long[size];
	    for (int i=0; i<size-1; i++){
		try{
		    values[i] = (long)(((Short)o[i]).shortValue());
		}
		catch (Exception e){
		    setError("Attempt to set short value (" + v + ") for column " + column + " failed. (addColumnElement)");
		    return false;
		}
	    }		    
	}
	
	values[size-1] = (long)v;
	columnObject[column].setValues(values, pcount, 1);
	return true;
    }
    
    /**
     * add a single element to a column of data
     *
     * @param          name        Column name 
     * @param          v           value to add to the column
     *
     * @return         <code>true</code> if successful<br>
     *                 <code>false</code> if an error occurs
     */
    public boolean addColumnElement(String name, short v)
    {
	return addColumnElement(getColumnIndex(name)+1, v);	
    }

    /**
     * Write a new row of column data.  Will write the latest data added to each column.
     *
     * @return          <code>true</code> if successful<br>
     *                 <code>false</code> if an error occurs
     */
    public boolean writeRow()
    {
	int i, j, row;
	int page = pageCount();
	// check that all columns are updated to the same row
	row = columnObject[0].getRowCount(page);
	for (i=1;i<columnCount;i++){
	    j = columnObject[i].getRowCount(page);
	    if (row != j){
		setError("row counts for columns are not consistent (column " + i + " has " + j + " rows, column 1 has " + row + " rows)");
		return false;
	    }
	}   
	if (asciiFile) {
	    if (columnCount > 0) {
		for (i=0;i<columnCount;i++) {
		    if (!columnObject[i].writeElement(dos, page, row, true)) {
			setError("invalid column elemant at page (" + page + ") row (" + row + ") for column " + columnObject[i].getName() + " (writeRow)");
			try {
			    dos.flush();
			} catch (IOException e) {
			    return false;
			}
			return false;
		    }
		}
		try {
		    dos.writeBytes("\n");
		} catch (IOException e) {
		    setError("unable to write newline character to ascii page " + page + " (writeRow)");
		    return false;
		} 
	    }
	    try {
		dos.flush();
	    } catch (IOException e) {
		setError("unable to flush data for ascii page " + page + " (writeRow)");
		return false;
	    }
	} else {
	    setError("unable to use writePage for binary data " + page + " (writeRow)");
	    return false;
	}	
	return true;
    }

    /**
     * Append to the error description array.
     * 
     * @param error_text        error message
     */
    public void setError(String error_text) {
	if (error_text != null) {
	    n_errors++;
	    if (error_description == null) {
            	error_description = new String[1];
	    } else {
	        error_description = (String[])SDDSUtil.resize(error_description, n_errors);
	    }
	    error_description[n_errors - 1] = error_text;
	}
    }

    /**
     * Return all the error messages.
     */
    public String getErrors() {
	int i, depth;
	String s = new String("");
	if (n_errors == 0)
	    return s;
	depth = n_errors;
	if (error_description == null) {
	    s = "warning: internal error: error_description variable is unexpectedly null\n";
	} else {
	    for ( i=depth-1 ; i>=0 ; i--) {
		if (error_description[i] == null) {
		    s = s + "warning: internal error: error_description[" + i + "] variable is unexpectedly null\n";
		} else {
		    s = s + error_description[i] + "\n";
 		}
	    }
	}
	n_errors = 0;
	return s;
    }
    /**
     * Print the error messages to standard error.
     */
    public void printErrors() {
	if (n_errors == 0)
	    return;
	System.err.print("Error:\n" + getErrors());
    }
}

