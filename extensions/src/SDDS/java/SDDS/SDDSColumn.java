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
//import hb.format.*;

/**
 * The SDDS Column object is used by the SDDS File object.
 * It was not the intention of the author to use this object
 * directly but it is still supported. After initializing the
 * SDDS Column the setupColumn method must be used to
 * give the column a name and a valid data type (long, short, 
 * float, double, string, character). The setValues method is
 * used next to give the column values for different
 * pages. The writeValues method or the writeElement method 
 * can be used to print the column values for a particular page.
 *
 * @author    Robert Soliday <a href=mailto:soliday@aps.anl.gov>soliday@aps.anl.gov</a>
 * @version   1.0 (November 1, 1999)
 * @since JDK 1.2
 */
public class SDDSColumn {
    String columnName;
    String columnSymbol;
    String columnUnits;
    String columnDescription;
    String columnFormatString;
    int columnType;
    int columnFieldLength;
    long[][] longValue;
    double[][] doubleValue;
    String[][] stringValue;
    char[][] characterValue;
    int pageCount;
    int[] rowCount;
    final int SDDS_SHORT = 4;
    final int SDDS_LONG = 3;
    final int SDDS_FLOAT = 2;
    final int SDDS_DOUBLE = 1;
    final int SDDS_STRING = 5;
    final int SDDS_CHARACTER = 6;

    /**
     * Create a new SDDS column that can be used to store column information for SDDS files.
     */
    public SDDSColumn() {
	pageCount = 0;
	columnType = 0;
    }

    /**
     * Set up the column information. 
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
     */
    public void setupColumn(String name, String symbol, String units, String description, String format_string, int type, int field_length) {
	columnName = name;
	columnSymbol = symbol;
	columnUnits = units;
	columnDescription = description;
	columnFormatString = format_string;
	columnType = type;
	columnFieldLength = field_length;
    }

    /**
     * Returns the name of the column
     *
     * @return   The <code>String</code> value of the column name.
     */
    public String getName() {
	return columnName;
    }

    /**
     * Set the name of the column
     *
     * @param name            name of the column
     */
    public void setName(String name) {
	columnName = name;
    }

    /**
     * Returns the symbol of the column
     *
     * @return   The <code>String</code> value of the column symbol.
     */
    public String getSymbol() {
	return columnSymbol;
    }

    /**
     * Set the symbol of the column
     *
     * @param symbol          symbol of the column
     */
    public void setSymbol(String symbol) {
	columnSymbol = symbol;
    }

    /**
     * Returns the units of the column
     *
     * @return   The <code>String</code> value of the column units.
     */
    public String getUnits() {
	return columnUnits;
    }

    /**
     * Set the units of the column
     *
     * @param units           units of the column
     */
    public void setUnits(String units) {
	columnUnits = units;
    }

    /**
     * Returns the description of the column
     *
     * @return   The <code>String</code> value of the column description.
     */
    public String getDescription() {
	return columnDescription;
    }

    /**
     * Set the description of the column
     *
     * @param description     description of the column
     */
    public void setDescription(String description) {
	columnDescription = description;
    }

    /**
     * Returns the format string of the column
     *
     * @return   The <code>String</code> value of the column format string.
     */
    public String getFormat_String() {
	return columnFormatString;
    }

    /**
     * Set the format string of the column
     *
     * @param format_string     format string of the column
     */
    public void setFormat_String(String format_string) {
	columnFormatString = format_string;
    }

    /**
     * Returns the type of the column
     *
     * @return   The <code>int</code> value of the column type.
     */
    public int getType() {
	return columnType;
    }

    /**
     * Returns the field length of the column
     *
     * @return   The <code>int</code> value of the column field length.
     */
    public int getField_Length() {
	return columnFieldLength;
    }

    /**
     * Set the field length of the column
     *
     * @param field_length      field length of the column
     */
    public void setField_Length(int field_length) {
	columnFieldLength = field_length;
    }

    /**
     * Returns the number of pages that the column contains
     *
     * @return   The <code>int</code> value of the number of pages the column contains.
     */
    public int getPageCount() {
	return pageCount;
    }

    /**
     * Returns the number of rows a page contains
     *
     * @return   The <code>int</code> value of the number of pages the column contains.<br>
     *           <code>-1</code> if the page value is invalid.
     */
    public int getRowCount(int page) {
	int i;
	if (rowCount == null)
	    return -1;
	i = Array.getLength(rowCount);
	if (page > i)
	    return -1;
	return rowCount[page-1];
    }

    /**
     * Inserts a blank page at the specified index
     * 
     * @param page             index of page to create insert
     * @return                 <code>true</code> on success<br>
     *                         <code>false</code> on error.
     */
    public boolean insertPage(int page) {
	if (page == pageCount+1) {
	    makeBlankPages(page);
	    return true;
	} else if ((page < 1) || (page > pageCount+1)) {
	    return false;
	} else {
	    int i;
	    switch (columnType) {
	    case SDDS_SHORT:
	    case SDDS_LONG:
		longValue = (long[][])SDDSUtil.resize(longValue, pageCount+1);
		for (i=pageCount;i>=page;i--) {
		    if (longValue[i] == null)
			longValue[i] = new long[rowCount[i-1]];
		    else 
			longValue[i] = (long[])SDDSUtil.resize(longValue[i], rowCount[i-1]);
		    longValue[i] = longValue[i-1];
		}
		longValue[page-1] = null;
		break;
	    case SDDS_FLOAT:
	    case SDDS_DOUBLE:
		doubleValue = (double[][])SDDSUtil.resize(doubleValue, pageCount+1);
		for (i=pageCount;i>=page;i--) {
		    if (doubleValue[i] == null)
			doubleValue[i] = new double[rowCount[i-1]];
		    else 
			doubleValue[i] = (double[])SDDSUtil.resize(doubleValue[i], rowCount[i-1]);
		    doubleValue[i] = doubleValue[i-1];
		}
		doubleValue[page-1] = null;
		break;
	    case SDDS_STRING:
		stringValue = (String[][])SDDSUtil.resize(stringValue, pageCount+1);
		for (i=pageCount;i>=page;i--) {
		    if (stringValue[i] == null)
			stringValue[i] = new String[rowCount[i-1]];
		    else 
			stringValue[i] = (String[])SDDSUtil.resize(stringValue[i], rowCount[i-1]);
		    stringValue[i] = stringValue[i-1];
		}
		stringValue[page-1] = null;
		break;
	    case SDDS_CHARACTER:
		characterValue = (char[][])SDDSUtil.resize(characterValue, pageCount+1);
		for (i=pageCount;i>=page;i--) {
		    if (characterValue[i] == null)
			characterValue[i] = new char[rowCount[i-1]];
		    else 
			characterValue[i] = (char[])SDDSUtil.resize(characterValue[i], rowCount[i-1]);
		    characterValue[i] = characterValue[i-1];
		}
		characterValue[page-1] = null;
		break;
	    default:
		return false;
	    }
	    rowCount = (int[])SDDSUtil.resize(rowCount, pageCount+1);
	    for (i=pageCount;i>=page;i--)
		rowCount[i] = rowCount[i-1];
	    rowCount[page-1] = 0;
	    pageCount++;
	    return true;
	}
    }

    /**
     * Delete a page at the specified index
     * 
     * @param page             index of page to delete
     * @return                 <code>true</code> on success<br>
     *                         <code>false</code> on error.
     */
    public boolean deletePage(int page) {
	if ((page < 1) || (page > pageCount)) {
	    return false;
	} else {
	    int i;
	    switch (columnType) {
	    case SDDS_SHORT:
	    case SDDS_LONG:
		for (i=page;i<pageCount;i++) {
		    if (longValue[i-1] == null)
			longValue[i-1] = new long[rowCount[i]];
		    else 
			longValue[i-1] = (long[])SDDSUtil.resize(longValue[i-1], rowCount[i]);
		    longValue[i-1] = longValue[i];
		}
		longValue = (long[][])SDDSUtil.resize(longValue, pageCount-1);
		break;
	    case SDDS_FLOAT:
	    case SDDS_DOUBLE:
		for (i=page;i<pageCount;i++) {
		    if (doubleValue[i-1] == null)
			doubleValue[i-1] = new double[rowCount[i]];
		    else 
			doubleValue[i-1] = (double[])SDDSUtil.resize(doubleValue[i-1], rowCount[i]);
		    doubleValue[i-1] = doubleValue[i];
		}
		doubleValue = (double[][])SDDSUtil.resize(doubleValue, pageCount-1);
		break;
	    case SDDS_STRING:
		for (i=page;i<pageCount;i++) {
		    if (stringValue[i-1] == null)
			stringValue[i-1] = new String[rowCount[i]];
		    else 
			stringValue[i-1] = (String[])SDDSUtil.resize(stringValue[i-1], rowCount[i]);
		    stringValue[i-1] = stringValue[i];
		}
		stringValue = (String[][])SDDSUtil.resize(stringValue, pageCount-1);
		break;
	    case SDDS_CHARACTER:
		for (i=page;i<pageCount;i++) {
		    if (characterValue[i-1] == null)
			characterValue[i-1] = new char[rowCount[i]];
		    else 
			characterValue[i-1] = (char[])SDDSUtil.resize(characterValue[i-1], rowCount[i]);
		    characterValue[i-1] = characterValue[i];
		}
		characterValue = (char[][])SDDSUtil.resize(characterValue, pageCount-1);
		break;
	    default:
		return false;
	    }
	    for (i=page;i<pageCount;i++)
		rowCount[i-1] = rowCount[i];
	    rowCount = (int[])SDDSUtil.resize(rowCount, pageCount-1);
	    pageCount--;
	    return true;
	}
    }

    /**
     * Create blank pages up to the last page specified
     * 
     * @param lastpage         last page to create blank page
     */
    public void makeBlankPages(int lastpage) {
	int n;
	n = lastpage - pageCount;
	if (n > 0) {
	    switch (columnType) {
	    case SDDS_SHORT:
	    case SDDS_LONG:
		if (longValue == null)
		    longValue = new long[n][];
		else if (Array.getLength(longValue) < lastpage)
		    longValue = (long[][])SDDSUtil.resize(longValue, lastpage);
		break;
	    case SDDS_FLOAT:
	    case SDDS_DOUBLE:
		if (doubleValue == null)
		    doubleValue = new double[n][];
		else if (Array.getLength(doubleValue) < lastpage)
		    doubleValue = (double[][])SDDSUtil.resize(doubleValue, lastpage);
	    case SDDS_STRING:
		if (stringValue == null)
		    stringValue = new String[n][];
		else if (Array.getLength(stringValue) < lastpage)
		    stringValue = (String[][])SDDSUtil.resize(stringValue, lastpage);
	    case SDDS_CHARACTER:
		if (characterValue == null)
		    characterValue = new char[n][];
		else if (Array.getLength(characterValue) < lastpage)
		    characterValue = (char[][])SDDSUtil.resize(characterValue, lastpage);
	    default:
		return;
	    }
	    if (rowCount == null)
		rowCount = new int[lastpage];
	    else if (Array.getLength(rowCount) < lastpage)
		rowCount = (int[])SDDSUtil.resize(rowCount, lastpage);
	    pageCount = lastpage;
	}
    }

    /**
     * Deletes the value of the column at a chosen page and at a chosen row
     *
     * @param page            the page of the column, must be greater then <code>0</code> 
     * @param row             the row of the column, must be greater then <code>0</code> 
     */
    public void deleteRow(int page, int row) {
	if ((page < 1) || (row < 1)) return;
	int i, n, rows;
	if (rowCount == null)
	    return;
	i = Array.getLength(rowCount);
	if (page > i)
	    return;
	page--;
	rows = rowCount[page];
	if (rows == 0) return;
	switch (columnType) {
	case SDDS_SHORT:
	case SDDS_LONG:
	    for (n=row; n<rows; n++) {
		longValue[page][n-1] = longValue[page][n];		
	    }
	    break;
	case SDDS_FLOAT:
	case SDDS_DOUBLE:
	    for (n=row; n<rows; n++) {
		doubleValue[page][n-1] = doubleValue[page][n];
	    }
	    break;
	case SDDS_STRING:
	    for (n=row; n<rows; n++) {
		stringValue[page][n-1] = stringValue[page][n];
	    }
	    break;
	case SDDS_CHARACTER:
	    for (n=row; n<rows; n++) {
		characterValue[page][n-1] = characterValue[page][n];
	    }
	    break;
	default:
	    return;
	}
	rowCount[page]--;
    }

    /**
     * Set the values of the column at a chosen page starting at the chosen row
     *
     * @param v               a long precision number array
     * @param page            the page of the column, must be greater then <code>0</code> 
     * @param startRow        the row of the column, must be greater then <code>0</code> 
     */
    public void setValues(long v[], int page, int startRow) {
	int n, rowsNeeded, arrayLength, i;
	arrayLength = Array.getLength(v);
	if ((page < 1) || (startRow < 1)) return;
	startRow--;
	n = page - pageCount;
	page--;
	rowsNeeded = arrayLength + startRow;
	switch (columnType) {
	case SDDS_SHORT:
	case SDDS_LONG:
	    if (n > 0)
		if (longValue == null)
		    longValue = new long[n][];
		else if (Array.getLength(longValue) < page+1)
		    longValue = (long[][])SDDSUtil.resize(longValue, page+1);
	    if (longValue[page] == null)
		longValue[page] = new long[rowsNeeded];
	    else if (Array.getLength(longValue[page]) < rowsNeeded)
		    longValue[page] = (long[])SDDSUtil.resize(longValue[page], rowsNeeded);
	    System.arraycopy(v,0,longValue[page],startRow,arrayLength);
	    break;
	case SDDS_FLOAT:
	case SDDS_DOUBLE:
	    if (n > 0)
		if (doubleValue == null)
		    doubleValue = new double[n][];
		else if (Array.getLength(doubleValue) < page+1)
		    doubleValue = (double[][])SDDSUtil.resize(doubleValue, page+1);
	    if (doubleValue[page] == null)
		doubleValue[page] = new double[rowsNeeded];
	    else if (Array.getLength(doubleValue[page]) < rowsNeeded)
		    doubleValue[page] = (double[])SDDSUtil.resize(doubleValue[page], rowsNeeded);
	    for (i=0;i<arrayLength;i++)
		doubleValue[page][i+startRow] = (double)v[i];
	    break;
	case SDDS_STRING:
	    if (n > 0)
		if (stringValue == null)
		    stringValue = new String[n][];
		else if (Array.getLength(stringValue) < page+1)
		    stringValue = (String[][])SDDSUtil.resize(stringValue, page+1);
	    if (stringValue[page] == null)
		stringValue[page] = new String[rowsNeeded];
	    else if (Array.getLength(stringValue[page]) < rowsNeeded)
		    stringValue[page] = (String[])SDDSUtil.resize(stringValue[page], rowsNeeded);
	    for (i=0;i<arrayLength;i++)
		stringValue[page][i+startRow] = Long.toString(v[i]);
	    break;
	case SDDS_CHARACTER:
	    if (n > 0)
		if (characterValue == null)
		    characterValue = new char[n][];
		else if (Array.getLength(characterValue) < page+1)
		    characterValue = (char[][])SDDSUtil.resize(characterValue, page+1);
	    if (characterValue[page] == null)
		characterValue[page] = new char[rowsNeeded];
	    else if (Array.getLength(characterValue[page]) < rowsNeeded)
		    characterValue[page] = (char[])SDDSUtil.resize(characterValue[page], rowsNeeded);
	    for (i=0;i<arrayLength;i++)
		characterValue[page][i+startRow] = (Long.toString(v[i])).charAt(0);
	    break;
	default:
	    return;
	}
	if (rowCount == null)
	    rowCount = new int[page+1];
	else if (Array.getLength(rowCount) < page+1)
	    rowCount = (int[])SDDSUtil.resize(rowCount, page+1);
	if (rowCount[page] < rowsNeeded)
	    rowCount[page] = rowsNeeded;
	if (n > 0)
	    pageCount = page+1;
	return;
    }

    /**
     * Set the values of the column at a chosen page starting at the chosen row
     *
     * @param v               a double precision floating-point number array
     * @param page            the page of the column, must be greater then <code>0</code> 
     * @param startRow        the row of the column, must be greater then <code>0</code> 
     */
    public void setValues(double v[], int page, int startRow) {
	int n, rowsNeeded, arrayLength, i;
	arrayLength = Array.getLength(v);
	if ((page < 1) || (startRow < 1)) return;
	startRow--;
	n = page - pageCount;
	page--;
	rowsNeeded = arrayLength + startRow;
	switch (columnType) {
	case SDDS_SHORT:
	case SDDS_LONG:
	    if (n > 0)
		if (longValue == null)
		    longValue = new long[n][];
		else if (Array.getLength(longValue) < page+1)
		    longValue = (long[][])SDDSUtil.resize(longValue, page+1);
	    if (longValue[page] == null)
		longValue[page] = new long[rowsNeeded];
	    else if (Array.getLength(longValue[page]) < rowsNeeded)
		    longValue[page] = (long[])SDDSUtil.resize(longValue[page], rowsNeeded);
	    for (i=0;i<arrayLength;i++)
		longValue[page][i+startRow] = (long)v[i];
	    break;
	case SDDS_FLOAT:
	case SDDS_DOUBLE:
	    if (n > 0)
		if (doubleValue == null)
		    doubleValue = new double[n][];
		else if (Array.getLength(doubleValue) < page+1)
		    doubleValue = (double[][])SDDSUtil.resize(doubleValue, page+1);
	    if (doubleValue[page] == null)
		doubleValue[page] = new double[rowsNeeded];
	    else if (Array.getLength(doubleValue[page]) < rowsNeeded)
		    doubleValue[page] = (double[])SDDSUtil.resize(doubleValue[page], rowsNeeded);
	    System.arraycopy(v,0,doubleValue[page],startRow,arrayLength);
	    break;
	case SDDS_STRING:
	    if (n > 0)
		if (stringValue == null)
		    stringValue = new String[n][];
		else if (Array.getLength(stringValue) < page+1)
		    stringValue = (String[][])SDDSUtil.resize(stringValue, page+1);
	    if (stringValue[page] == null)
		stringValue[page] = new String[rowsNeeded];
	    else if (Array.getLength(stringValue[page]) < rowsNeeded)
		    stringValue[page] = (String[])SDDSUtil.resize(stringValue[page], rowsNeeded);
	    for (i=0;i<arrayLength;i++)
		stringValue[page][i+startRow] = Double.toString(v[i]);
	    break;
	case SDDS_CHARACTER:
	    if (n > 0)
		if (characterValue == null)
		    characterValue = new char[n][];
		else if (Array.getLength(characterValue) < page+1)
		    characterValue = (char[][])SDDSUtil.resize(characterValue, page+1);
	    if (characterValue[page] == null)
		characterValue[page] = new char[rowsNeeded];
	    else if (Array.getLength(characterValue[page]) < rowsNeeded)
		    characterValue[page] = (char[])SDDSUtil.resize(characterValue[page], rowsNeeded);
	    for (i=0;i<arrayLength;i++)
		characterValue[page][i+startRow] = (Double.toString(v[i])).charAt(0);
	    break;
	default:
	    return;
	}
	if (rowCount == null)
	    rowCount = new int[page+1];
	else if (Array.getLength(rowCount) < page+1)
	    rowCount = (int[])SDDSUtil.resize(rowCount, page+1);
	if (rowCount[page] < rowsNeeded)
	    rowCount[page] = rowsNeeded;
	if (n > 0)
	    pageCount = page+1;
	return;
    }

    /**
     * Set the values of the column at a chosen page starting at the chosen row
     *
     * @param v               a string value array
     * @param page            the page of the column, must be greater then <code>0</code> 
     * @param startRow        the row of the column, must be greater then <code>0</code> 
     */
    public void setValues(String v[], int page, int startRow) {
	int n, rowsNeeded, arrayLength, i;
	arrayLength = Array.getLength(v);
	if ((page < 1) || (startRow < 1)) return;
	startRow--;
	n = page - pageCount;
	page--;
	rowsNeeded = arrayLength + startRow;
	switch (columnType) {
	case SDDS_SHORT:
	case SDDS_LONG:
	    if (n > 0)
		if (longValue == null)
		    longValue = new long[n][];
		else if (Array.getLength(longValue) < page+1)
		    longValue = (long[][])SDDSUtil.resize(longValue, page+1);
	    if (longValue[page] == null)
		longValue[page] = new long[rowsNeeded];
	    else if (Array.getLength(longValue[page]) < rowsNeeded)
		    longValue[page] = (long[])SDDSUtil.resize(longValue[page], rowsNeeded);
	    for (i=0;i<arrayLength;i++) {
	        try {
		    longValue[page][i+startRow] = (Long.valueOf(v[i])).longValue();
	        } catch (Exception e) {
		    longValue[page][i+startRow] = (long)0;
	        }
	    }
	    break;
	case SDDS_FLOAT:
	case SDDS_DOUBLE:
	    if (n > 0)
		if (doubleValue == null)
		    doubleValue = new double[n][];
		else if (Array.getLength(doubleValue) < page+1)
		    doubleValue = (double[][])SDDSUtil.resize(doubleValue, page+1);
	    if (doubleValue[page] == null)
		doubleValue[page] = new double[rowsNeeded];
	    else if (Array.getLength(doubleValue[page]) < rowsNeeded)
		    doubleValue[page] = (double[])SDDSUtil.resize(doubleValue[page], rowsNeeded);
	    for (i=0;i<arrayLength;i++) {
	        try {
		    doubleValue[page][i+startRow] = (Double.valueOf(v[i])).doubleValue();
	        } catch (Exception e) {
		    doubleValue[page][i+startRow] = (double)0;
	        }
	    }
	    break;
	case SDDS_STRING:
	    if (n > 0)
		if (stringValue == null)
		    stringValue = new String[n][];
		else if (Array.getLength(stringValue) < page+1)
		    stringValue = (String[][])SDDSUtil.resize(stringValue, page+1);
	    if (stringValue[page] == null)
		stringValue[page] = new String[rowsNeeded];
	    else if (Array.getLength(stringValue[page]) < rowsNeeded)
		    stringValue[page] = (String[])SDDSUtil.resize(stringValue[page], rowsNeeded);
	    System.arraycopy(v,0,stringValue[page],startRow,arrayLength);
	    break;
	case SDDS_CHARACTER:
	    if (n > 0)
		if (characterValue == null)
		    characterValue = new char[n][];
		else if (Array.getLength(characterValue) < page+1)
		    characterValue = (char[][])SDDSUtil.resize(characterValue, page+1);
	    if (characterValue[page] == null)
		characterValue[page] = new char[rowsNeeded];
	    else if (Array.getLength(characterValue[page]) < rowsNeeded)
		    characterValue[page] = (char[])SDDSUtil.resize(characterValue[page], rowsNeeded);
	    for (i=0;i<arrayLength;i++) {
	        if (v[i].length() > 0) {
		    characterValue[page][i+startRow] = v[i].charAt(0);
	        } else {
		    characterValue[page][i+startRow] = '0';
	        }
	    }
	    break;
	default:
	    return;
	}
	if (rowCount == null)
	    rowCount = new int[page+1];
	else if (Array.getLength(rowCount) < page+1)
	    rowCount = (int[])SDDSUtil.resize(rowCount, page+1);
	if (rowCount[page] < rowsNeeded)
	    rowCount[page] = rowsNeeded;
	if (n > 0)
	    pageCount = page+1;
	return;
    }

    /**
     * Write the header to a file.
     * 
     * @param dos         a valid DataOutputStream
     * @return            <code>true</code> if the header was successfully written<br>
     *                    <code>false</code> if an error occured
     */
    public boolean writeHeader(DataOutputStream dos) {
	String line;
	if (columnName == null) return false;
	line = "&column name=" + columnName + ", ";
	if (columnSymbol != null) {
	    line = line.concat("symbol=" + SDDSUtil.prepareString(columnSymbol) + ", ");
	}
	if (columnUnits != null) {
	    line = line.concat("units=" + SDDSUtil.prepareString(columnUnits) + ", ");
	}
	if (columnDescription != null) {
	    line = line.concat("description=" + SDDSUtil.prepareString(columnDescription) + ", ");
	}
	if (columnFormatString != null) {
	    line = line.concat("format_string=" + SDDSUtil.prepareString(columnFormatString) + ", ");
	}
        line = line.concat("type=" + SDDSUtil.getTypeName(columnType) + ", ");
	line = line.concat("&end\n");
	try {
	    dos.writeBytes(line);
	} catch (Exception e) {
	    return false;
	}
	return true;
    }

    /**
     * Write the value of the column for a chosen page and row
     *
     * @param dos             a valid DataOutputStream
     * @param page            the page of the column, must be greater then <code>0</code>
     * @param row             the row of the column, must be greater then <code>0</code>
     * @param ascii           set <code>true</code> to write ascii column data, 
     *                        <code>false</code> to write big endian binary column data
     * @return                <code>true</code> if the value was successfully written<br>
     *                        <code>false</code> if an invalid page or row was encountered or if
     *                        the column type is undefined.
     */
    public boolean writeElement(DataOutputStream dos, int page, int row, boolean ascii) {
	if ((pageCount == 0) || (page < 1) || (page > pageCount) || (row < 1) || (row > rowCount[page-1]))
	    return false;
	page--;
	row--;
	try {
	    if (ascii) {
		switch (columnType) {
		case SDDS_SHORT:
		    dos.writeBytes((short)longValue[page][row] + " ");
		    break;
		case SDDS_LONG:
		    dos.writeBytes(longValue[page][row] + " ");
		    break;
		case SDDS_FLOAT:
		    dos.writeBytes((float)doubleValue[page][row] + " ");
		    break;
		case SDDS_DOUBLE:
		    dos.writeBytes(doubleValue[page][row] + " ");
		    break;
		case SDDS_STRING:
		    if (stringValue[page][row] == null)
			stringValue[page][row] = "";
		    dos.writeBytes(SDDSUtil.prepareString(stringValue[page][row]) + " ");
		    break;
		case SDDS_CHARACTER:
		    if (Character.isLetterOrDigit(characterValue[page][row])) {
			dos.writeBytes(characterValue[page][row] + " ");
		    } else {
			String temp;
			temp = Integer.toOctalString(characterValue[page][row]);
			while (temp.length() < 3)
			    temp = "0" + temp;
			dos.writeBytes("\\" + temp + " ");
		    }
		    break;
		default:
		    return false;
		}
	    } else {
		switch (columnType) {
		case SDDS_SHORT:
		    dos.writeShort((short)longValue[page][row]);
		    break;
		case SDDS_LONG: /* note that to be compatable with existing software we must write long values as an int */
		    dos.writeInt((int)longValue[page][row]);
		    break;
		case SDDS_FLOAT:
		    dos.writeFloat((float)doubleValue[page][row]);
		    break;
		case SDDS_DOUBLE:
		    dos.writeDouble(doubleValue[page][row]);
		    break;
		case SDDS_STRING:
		    if (stringValue[page][row] == null) {
			dos.writeInt(0);
		    } else {
			dos.writeInt(stringValue[page][row].length());
			dos.writeBytes(stringValue[page][row]);
		    }
		    break;
		case SDDS_CHARACTER: /* note that to be compatable with existing software we must use writeByte instead of writeChar */
		    dos.writeByte((int)characterValue[page][row]);
		    break;
		default:
		    return false;
		}
	    }
	} catch  (Exception e) {
	    return false;
	}
	return true;
    }

    /**
     * Write the little endian binary value of the column for a chosen page and row
     *
     * @param ledos           a valid LEDataOutputStream
     * @param page            the page of the column, must be greater then <code>0</code>
     * @param row             the row of the column, must be greater then <code>0</code>
     * @return                <code>true</code> if the value was successfully written<br>
     *                        <code>false</code> if an invalid page or row was encountered or if
     *                        the column type is undefined.
     */
    public boolean writeElement(LEDataOutputStream ledos, int page, int row) {
	if ((pageCount == 0) || (page < 1) || (page > pageCount) || (row < 1) || (row > rowCount[page-1]))
	    return false;
	page--;
	row--;
	try {
	    switch (columnType) {
	    case SDDS_SHORT:
	        ledos.writeShort((short)longValue[page][row]);
	        break;
	    case SDDS_LONG: /* note that to be compatable with existing software we must write long values as an int */
	        ledos.writeInt((int)longValue[page][row]);
	        break;
	    case SDDS_FLOAT:
	        ledos.writeFloat((float)doubleValue[page][row]);
	        break;
	    case SDDS_DOUBLE:
	        ledos.writeDouble(doubleValue[page][row]);
	        break;
	    case SDDS_STRING:
		if (stringValue[page][row] == null) {
		    ledos.writeInt(0);
	        } else {
		    ledos.writeInt(stringValue[page][row].length());
	            ledos.writeBytes(stringValue[page][row]);
		}
	        break;
	    case SDDS_CHARACTER: /* note that to be compatable with existing software we must use writeByte instead of writeChar */
	        ledos.writeByte((int)characterValue[page][row]);
	        break;
	    default:
	        return false;
	    }
	} catch  (IOException e) {
	    return false;
	}
	return true;
    }

    /**
     * Write the values of the column for a chosen page
     *
     * @param dos             a valid DataOutputStream
     * @param page            the page of the column, must be greater then <code>0</code>
     * @return                <code>true</code> if the values were successfully written<br>
     *                        <code>false</code> if an invalid page was encountered or if
     *                        the column type is undefined.
     */
    public boolean writeColumn(DataOutputStream dos, int page) {
	int i,j;
	if ((pageCount == 0) || (page < 1) || (page > pageCount))
	    return false;
	page--;
	try {
	    switch (columnType) {
	    case SDDS_SHORT:
		j = Array.getLength(longValue[page]);
		for (i=0;i<j;i++) 
		    dos.writeBytes((short)longValue[page][i] + "\n");
		break;
	    case SDDS_LONG:
		j = Array.getLength(longValue[page]);
		for (i=0;i<j;i++) 
		    dos.writeBytes(longValue[page][i] + "\n");
		break;
	    case SDDS_FLOAT:
		j = Array.getLength(doubleValue[page]);
		for (i=0;i<j;i++) 
		    dos.writeBytes((float)doubleValue[page][i] + "\n");
		break;
	    case SDDS_DOUBLE:
		j = Array.getLength(doubleValue[page]);
		for (i=0;i<j;i++) 
		    dos.writeBytes(doubleValue[page][i] + "\n");
		break;
	    case SDDS_STRING:
		j = Array.getLength(stringValue[page]);
		for (i=0;i<j;i++) {
		    if (stringValue[page][i] == null)
			stringValue[page][i] = "";
		    dos.writeBytes(SDDSUtil.prepareString(stringValue[page][i]) + "\n");
		}
		break;
	    case SDDS_CHARACTER:
		String temp;
		j = Array.getLength(characterValue[page]);
		for (i=0;i<j;i++) {
		    if (Character.isLetterOrDigit(characterValue[page][i])) {
			dos.writeBytes(characterValue[page][i] + "\n");
		    } else {
			temp = Integer.toOctalString(characterValue[page][i]);
			while (temp.length() < 3)
			    temp = "0" + temp;
			dos.writeBytes("\\" + temp + "\n");
		    }
		}
		break;
	    default:
		return false;
	    }
	} catch (Exception e) {
	    return false;
	}
	return true;
    }

    /**
     * Get the values of the column for a chosen page
     *
     * @param page            the page of the column, must be greater then <code>0</code>
     * @param string_quotes   <code>true</code> if quotes are to be added around strings<br>
     *                        <code>false</code> if raw strings are to be outputed.
     * @return                Object array of data
     */
    public Object[] getValues(int page, boolean string_quotes) {
	Object values[];
	int i,j;
	if ((pageCount == 0) || (page < 1) || (page > pageCount))
	    return null;
	page--;
	switch (columnType) {
	case SDDS_SHORT:
	    if (longValue[page] == null)
		return null;
	    j = Array.getLength(longValue[page]);
	    values = new Object[j];
	    for (i=0;i<j;i++) 
		values[i] = new Short((short)longValue[page][i]);
	    break;
	case SDDS_LONG:
	    if (longValue[page] == null)
		return null;
	    j = Array.getLength(longValue[page]);
	    values = new Object[j];
	    for (i=0;i<j;i++) 
		values[i] = new Long(longValue[page][i]);
	    break;
	case SDDS_FLOAT:
	    if (doubleValue[page] == null)
		return null;
	    j = Array.getLength(doubleValue[page]);
	    values = new Object[j];
	    for (i=0;i<j;i++) 
		values[i] = new Float((float)doubleValue[page][i]);
	    break;
	case SDDS_DOUBLE:
	    if (doubleValue[page] == null)
		return null;
	    j = Array.getLength(doubleValue[page]);
	    values = new Object[j];
	    for (i=0;i<j;i++) 
		values[i] = new Double(doubleValue[page][i]);
	    break;
	case SDDS_STRING:
	    if (stringValue[page] == null)
		return null;
	    j = Array.getLength(stringValue[page]);
	    values = new Object[j];
	    if (string_quotes) {
		for (i=0;i<j;i++) {
		    if (stringValue[page][i] == null)
			stringValue[page][i] = "";
		    values[i] = SDDSUtil.prepareString(stringValue[page][i]);
		}
	    } else {
		for (i=0;i<j;i++) {
		    if (stringValue[page][i] == null)
			stringValue[page][i] = "";
		    values[i] = stringValue[page][i];
		}
	    }
	    break;
	case SDDS_CHARACTER:
	    if (characterValue[page] == null)
		return null;
	    String temp;
	    j = Array.getLength(characterValue[page]);
	    values = new Object[j];
	    for (i=0;i<j;i++) {
		if (Character.isLetterOrDigit(characterValue[page][i])) {
		    values[i] = (new Character(characterValue[page][i])).toString();
		} else {
		    temp = Integer.toOctalString(characterValue[page][i]);
		    while (temp.length() < 3)
			temp = "0" + temp;
		    values[i] = "\\" + temp;
		}
	    }
	    break;
	default:
	    return null;
	}
	return values;
    }

}
