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

/**
 * The SDDS Parameter object is used by the SDDS File object.
 * It was not the intention of the author to use this object
 * directly but it is still supported. After initializing the
 * SDDS Parameter the setupParameter method must be used to
 * give the parameter a name and a valid data type (long, short, 
 * float, double, string, character). The setValue method is 
 * used next to give the parameter values for different
 * pages. The writeValue method can be used to print
 * the value for a particular page.
 *
 * @author    Robert Soliday <a href=mailto:soliday@aps.anl.gov>soliday@aps.anl.gov</a>
 * @version   1.0 (November 1, 1999)
 * @since JDK 1.2
 */
public class SDDSParameter {
    String parameterName;
    String parameterSymbol;
    String parameterUnits;
    String parameterDescription;
    String parameterFormatString;
    int parameterType;
    String parameterFixedValue;
    long[] longValue;
    double[] doubleValue;
    String[] stringValue;
    char[] characterValue;
    int pageCount;
    final int SDDS_SHORT = 4;
    final int SDDS_LONG = 3;
    final int SDDS_FLOAT = 2;
    final int SDDS_DOUBLE = 1;
    final int SDDS_STRING = 5;
    final int SDDS_CHARACTER = 6;
    boolean fixed;

    /**
     * Create a new SDDS parameter that can be used to store parameter information for SDDS files.
     */
    public SDDSParameter() {
	pageCount = 0;
	parameterType = 0;
	fixed = false;
    }

    /**
     * Set up the parameter information. 
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
     */
    public void setupParameter(String name, String symbol, String units, String description, String format_string, int type, String fixed_value) {
	parameterName = name;
	parameterSymbol = symbol;
	parameterUnits = units;
	parameterDescription = description;
	parameterFormatString = format_string;
	parameterType = type;
	parameterFixedValue = fixed_value;
	if (fixed_value == null)
	    fixed = false;
	else
	    fixed = true;
    }

    /**
     * Returns the name of the parameter
     *
     * @return   The <code>String</code> value of the parameter name.
     */
    public String getName() {
	return parameterName;
    }

    /**
     * Set the parameter name
     *
     * @param name            name of the parameter
     */
    public void setName(String name) {
        parameterName = name;
    }

    /**
     * Returns the symbol of the parameter
     *
     * @return   The <code>String</code> value of the parameter symbol.
     */
    public String getSymbol() {
	return parameterSymbol;
    }

    /**
     * Set the parameter symbol
     *
     * @param symbol          symbol of the parameter
     */
    public void setSymbol(String symbol) {
	parameterSymbol = symbol;
    }

    /**
     * Returns the units of the parameter
     *
     * @return   The <code>String</code> value of the parameter units.
     */
    public String getUnits() {
	return parameterUnits;
    }

    /**
     * Set the parameter units
     *
     * @param units           units of the parameter
     */
    public void setUnits(String units) {
	parameterUnits = units;
    }

    /**
     * Returns the description of the parameter
     *
     * @return   The <code>String</code> value of the parameter description.
     */
    public String getDescription() {
	return parameterDescription;
    }

    /**
     * Set the parameter description
     *
     * @param description     description of the parameter
     */
    public void setDescription(String description) {
	parameterDescription = description;
    }

    /**
     * Returns the format string of the parameter
     *
     * @return   The <code>String</code> value of the parameter format string.
     */
    public String getFormat_String() {
	return parameterFormatString;
    }

    /**
     * Set the parameter format string
     *
     * @param format_string   format string of the parameter
     */
    public void setFormat_String(String format_string) {
	parameterFormatString = format_string;
    }

    /**
     * Returns the type of the parameter
     *
     * @return   The <code>int</code> value of the parameter type.
     */
    public int getType() {
	return parameterType;
    }

    /**
     * Returns the fixed value of the parameter
     *
     * @return   The <code>String</code> value of the parameter fixed value.
     */
    public String getFixed_Value() {
	return parameterFixedValue;
    }

    /**
     * Set the fixed value of the parameter
     *
     * @param fixed_value     fixed_value of the parameter
     */
    public void setFixed_Value(String fixed_value) {
	parameterFixedValue = fixed_value;
	fixed = true;
    }

    /**
     * Returns the number of pages that the parameter contains
     *
     * @return   The <code>int</code> value of the number of pages the parameter contains.
     */
    public int getPageCount() {
	return pageCount;
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
	    if (parameterFixedValue != null) {
		pageCount++;
		return true;
	    }
	    int i;
	    switch (parameterType) {
	    case SDDS_SHORT:
	    case SDDS_LONG:
		longValue = (long[])SDDSUtil.resize(longValue, pageCount+1);
		for (i=pageCount;i>=page;i--) {
		    longValue[i] = longValue[i-1];
		}
		longValue[page-1] = 0;
		break;
	    case SDDS_FLOAT:
	    case SDDS_DOUBLE:
		doubleValue = (double[])SDDSUtil.resize(doubleValue, pageCount+1);
		for (i=pageCount;i>=page;i--) {
		    doubleValue[i] = doubleValue[i-1];
		}
		doubleValue[page-1] = 0;
		break;
	    case SDDS_STRING:
		stringValue = (String[])SDDSUtil.resize(stringValue, pageCount+1);
		for (i=pageCount;i>=page;i--) {
		    stringValue[i] = stringValue[i-1];
		}
		stringValue[page-1] = "";
		break;
	    case SDDS_CHARACTER:
		characterValue = (char[])SDDSUtil.resize(characterValue, pageCount+1);
		for (i=pageCount;i>=page;i--) {
		    characterValue[i] = characterValue[i-1];
		}
		characterValue[page-1] = '0';
		break;
	    default:
		return false;
	    }
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
	    if (parameterFixedValue != null) {
		pageCount--;
		return true;
	    }
	    switch (parameterType) {
	    case SDDS_SHORT:
	    case SDDS_LONG:
		for (i=page;i<pageCount;i++) {
		    longValue[i-1] = longValue[i];
		}
		longValue = (long[])SDDSUtil.resize(longValue, pageCount-1);
		break;
	    case SDDS_FLOAT:
	    case SDDS_DOUBLE:
		for (i=page;i<pageCount;i++) {
		    doubleValue[i-1] = doubleValue[i];
		}
		doubleValue = (double[])SDDSUtil.resize(doubleValue, pageCount-1);
		break;
	    case SDDS_STRING:
		for (i=page;i<pageCount;i++) {
		    stringValue[i-1] = stringValue[i];
		}
		stringValue = (String[])SDDSUtil.resize(stringValue, pageCount-1);
		break;
	    case SDDS_CHARACTER:
		for (i=page;i<pageCount;i++) {
		    characterValue[i-1] = characterValue[i];
		}
		characterValue = (char[])SDDSUtil.resize(characterValue, pageCount-1);
		break;
	    default:
		return false;
	    }
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
	    if (parameterFixedValue != null) {
		pageCount = lastpage;
		return;
	    }
	    switch (parameterType) {
	    case SDDS_SHORT:
	    case SDDS_LONG:
		if (longValue == null)
		    longValue = new long[n];
		else if (Array.getLength(longValue) < lastpage)
		    longValue = (long[])SDDSUtil.resize(longValue, lastpage);
		break;
	    case SDDS_FLOAT:
	    case SDDS_DOUBLE:
		if (doubleValue == null)
		    doubleValue = new double[n];
		else if (Array.getLength(doubleValue) < lastpage)
		    doubleValue = (double[])SDDSUtil.resize(doubleValue, lastpage);
		break;
	    case SDDS_STRING:
		if (stringValue == null)
		    stringValue = new String[n];
		else if (Array.getLength(stringValue) < lastpage)
		    stringValue = (String[])SDDSUtil.resize(stringValue, lastpage);
		break;
	    case SDDS_CHARACTER:
		if (characterValue == null)
		    characterValue = new char[n];
		else if (Array.getLength(characterValue) < lastpage)
		    characterValue = (char[])SDDSUtil.resize(characterValue, lastpage);
		break;
	    default:
		return;
	    }
	    pageCount = lastpage;
	}
    }

    /**
     * Set the value of the parameter at a chosen page
     *
     * @param value           a long precision number
     * @param page            the page of the parameter, must be greater then <code>0</code> 
     */
    public void setValue(long value, int page) {
	int n;
	Long v = new Long(value);
	if (page < 1) return;
	n = page - pageCount;
	if (parameterFixedValue != null) {
	    if (n > 0)
		pageCount = page;
	    return;
	}
	switch (parameterType) {
	case SDDS_SHORT:
	case SDDS_LONG:
	    if (n > 0) {
		if (longValue == null) {
		    longValue = new long[n];
		} else if (Array.getLength(longValue) < page) {
		    longValue = (long[])SDDSUtil.resize(longValue, page);
		}
	    }
	    longValue[page-1] = value;
	    break;
	case SDDS_FLOAT:
	case SDDS_DOUBLE:
	    if (n > 0) {
		if (doubleValue == null) {
		    doubleValue = new double[n];
		} else if (Array.getLength(doubleValue) < page) {
		    doubleValue = (double[])SDDSUtil.resize(doubleValue, page);
		}
	    }
	    doubleValue[page-1] = v.doubleValue();
	    break;
	case SDDS_STRING:
	    if (n > 0) {
		if (stringValue == null) {
		    stringValue = new String[n];
		} else if (Array.getLength(stringValue) < page) {
		    stringValue = (String[])SDDSUtil.resize(stringValue, page);
		}
	    }
	    stringValue[page-1] = v.toString();
	    break;
	case SDDS_CHARACTER:
	    String temp;
	    if (n > 0) {
		if (characterValue == null) {
		    characterValue = new char[n];
		} else if (Array.getLength(characterValue) < page) {
		    characterValue = (char[])SDDSUtil.resize(characterValue, page);
		}
	    }
	    temp = v.toString();
	    if (temp.length() > 0) {
		characterValue[page-1] = temp.charAt(0);
	    } else {
		characterValue[page-1] = '0';
	    }
	    break;
	default:
	    return;
	}
	if (n > 0)
	    pageCount = page;
	return;
    }

    /**
     * Set the value of the parameter at a chosen page
     *
     * @param value           a double precision floating-point number
     * @param page            the page of the parameter, must be greater then <code>0</code> 
     */
    public void setValue(double value, int page) {
	int n;
	Double v = new Double(value);
	if (page < 1) return;
	n = page - pageCount;
	if (parameterFixedValue != null) {
	    if (n > 0)
		pageCount = page;
	    return;
	}
	switch (parameterType) {
	case SDDS_SHORT:
	case SDDS_LONG:
	    if (n > 0) {
		if (longValue == null) {
		    longValue = new long[n];
		} else if (Array.getLength(longValue) < page) {
		    longValue = (long[])SDDSUtil.resize(longValue, page);
		}
	    }
	    longValue[page-1] = v.longValue();
	    break;
	case SDDS_FLOAT:
	case SDDS_DOUBLE:
	    if (n > 0) {
		if (doubleValue == null) {
		    doubleValue = new double[n];
		} else if (Array.getLength(doubleValue) < page) {
		    doubleValue = (double[])SDDSUtil.resize(doubleValue, page);
		}
	    }
	    doubleValue[page-1] = value;
	    break;
	case SDDS_STRING:
	    if (n > 0) {
		if (stringValue == null) {
		    stringValue = new String[n];
		} else if (Array.getLength(stringValue) < page) {
		    stringValue = (String[])SDDSUtil.resize(stringValue, page);
		}
	    }
	    stringValue[page-1] = v.toString();
	    break;
	case SDDS_CHARACTER:
	    String temp;
	    if (n > 0) {
		if (characterValue == null) {
		    characterValue = new char[n];
		} else if (Array.getLength(characterValue) < page) {
		    characterValue = (char[])SDDSUtil.resize(characterValue, page);
		}
	    }
	    temp = v.toString();
	    if (temp.length() > 0) {
		characterValue[page-1] = temp.charAt(0);
	    } else {
		characterValue[page-1] = '0';
	    }
	    break;
	default:
	    return;
	}
	if (n > 0)
	    pageCount = page;
	return;
    }

    /**
     * Set the value of the parameter at a chosen page
     *
     * @param value           a string value
     * @param page            the page of the parameter, must be greater then <code>0</code> 
     */
    public void setValue(String value, int page) {
	int n;
	if (page < 1) return;
	n = page - pageCount;
	if (parameterFixedValue != null) {
	    if (n > 0)
		pageCount = page;
	    return;
	}
	switch (parameterType) {
	case SDDS_SHORT:
	case SDDS_LONG:
	    if (n > 0) {
		if (longValue == null) {
		    longValue = new long[n];
		} else if (Array.getLength(longValue) < page) {
		    longValue = (long[])SDDSUtil.resize(longValue, page);
		}
	    }
	    try {
		longValue[page-1] = (Long.valueOf(value)).longValue();
	    } catch (Exception e) {
		longValue[page-1] = (long)0;
	    }
	    break;
	case SDDS_FLOAT:
	case SDDS_DOUBLE:
	    if (n > 0) {
		if (doubleValue == null) {
		    doubleValue = new double[n];
		} else if (Array.getLength(doubleValue) < page) {
		    doubleValue = (double[])SDDSUtil.resize(doubleValue, page);
		}
	    }
	    try {
		doubleValue[page-1] = (Double.valueOf(value)).doubleValue();
	    } catch (Exception e) {
		doubleValue[page-1] = (double)0;
	    }
	    break;
	case SDDS_STRING:
	    if (n > 0) {
		if (stringValue == null) {
		    stringValue = new String[n];
		} else if (Array.getLength(stringValue) < page) {
		    stringValue = (String[])SDDSUtil.resize(stringValue, page);
		}
	    }
	    stringValue[page-1] = value;
	    break;
	case SDDS_CHARACTER:
	    if (n > 0) {
		if (characterValue == null) {
		    characterValue = new char[n];
		} else if (Array.getLength(characterValue) < page) {
		    characterValue = (char[])SDDSUtil.resize(characterValue, page);
		}
	    }
	    if (value.length() > 0) {
		characterValue[page-1] = value.charAt(0);
	    } else {
		characterValue[page-1] = '0';
	    }
	    break;
	default:
	    return;
	}
	if (n > 0)
	    pageCount = page;
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
	if (parameterName == null) return false;
	line = "&parameter name=" + parameterName + ", ";
	if (parameterSymbol != null) {
	    line = line.concat("symbol=" + SDDSUtil.prepareString(parameterSymbol) + ", ");
	}
	if (parameterUnits != null) {
	    line = line.concat("units=" + SDDSUtil.prepareString(parameterUnits) + ", ");
	}
	if (parameterDescription != null) {
	    line = line.concat("description=" + SDDSUtil.prepareString(parameterDescription) + ", ");
	}
	if (parameterFormatString != null) {
	    line = line.concat("format_string=" + SDDSUtil.prepareString(parameterFormatString) + ", ");
	}
        line = line.concat("type=" + SDDSUtil.getTypeName(parameterType) + ", ");
	if (parameterFixedValue != null) {
	    line = line.concat("fixed_value=" + SDDSUtil.prepareString(parameterFixedValue) + ", ");
	}
	line = line.concat("&end\n");
	try {
	    dos.writeBytes(line);
	} catch (Exception e) {
	    return false;
	}
	return true;
    }

    /**
     * Write the value of the parameter for a chosen page
     *
     * @param dos             a valid DataOutputStream
     * @param page            the page of the parameter, must be greater then <code>0</code>
     * @param ascii           set <code>true</code> to write ascii parameters, 
     *                        <code>false</code> to write big endian binary parameters
     * @return                <code>true</code> if the value was successfully written<br>
     *                        <code>false</code> if an invalid page was encountered or if
     *                        the parameter type is undefined.
     */
    public boolean writeValue(DataOutputStream dos, int page, boolean ascii) {
	if (parameterFixedValue != null)
	    return true;
	if ((pageCount == 0) || (page < 1) || (page > pageCount))
	    return false;
	try {
	    if (ascii) {
		switch (parameterType) {
		case SDDS_SHORT:
		    dos.writeBytes(Short.toString((short)longValue[page-1]));
		    break;
		case SDDS_LONG:
		    dos.writeBytes(Long.toString(longValue[page-1]));
		    break;
		case SDDS_FLOAT:
		    dos.writeBytes(Float.toString((float)doubleValue[page-1]));
		    break;
		case SDDS_DOUBLE:
		    dos.writeBytes(Double.toString(doubleValue[page-1]));
		    break;
		case SDDS_STRING:
		    if (stringValue[page-1] == null)
			stringValue[page-1] = "";
		    dos.writeBytes(SDDSUtil.prepareString(stringValue[page-1]));
		    break;
		case SDDS_CHARACTER:
		    String temp;
		    if (Character.isLetterOrDigit(characterValue[page-1])) {
			dos.writeByte(characterValue[page-1]);
		    } else {
			temp = Integer.toOctalString(characterValue[page-1]);
			while (temp.length() < 3) {
			    temp = "0" + temp;
			}
			dos.writeBytes("\\" + temp);
		    }
		    break;
		default:
		    return false;
		}
		dos.writeBytes("\n");
	    } else {
		switch (parameterType) {
		case SDDS_SHORT:
		    dos.writeShort((short)longValue[page-1]);
		    break;
		case SDDS_LONG: /* note that to be compatable with existing software we must write long values as an int */
		    dos.writeInt((int)longValue[page-1]); 
		    break;
		case SDDS_FLOAT:
		    dos.writeFloat((float)doubleValue[page-1]);
		    break;
		case SDDS_DOUBLE:
		    dos.writeDouble(doubleValue[page-1]);
		    break;
		case SDDS_STRING:
		    if (stringValue[page-1] == null) {
			dos.writeInt(0);
		    } else {
			dos.writeInt(stringValue[page-1].length());
			dos.writeBytes(stringValue[page-1]);
		    }
		    break;
		case SDDS_CHARACTER: /* note that to be compatable with existing software we must use writeByte instead of writeChar */
		    dos.writeByte((int)characterValue[page-1]);
		    break;
		default:
		    return false;
		}
	    }
	} catch (IOException e) {
	    return false;
	}
	return true;
    }


    /**
     * Write the little endian binary value of the parameter for a chosen page
     *
     * @param ledos           a valid LEDataOutputStream
     * @param page            the page of the parameter, must be greater then <code>0</code>
     * @return                <code>true</code> if the value was successfully written<br>
     *                        <code>false</code> if an invalid page was encountered or if
     *                        the parameter type is undefined.
     */
    public boolean writeValue(LEDataOutputStream ledos, int page) {
	if (parameterFixedValue != null)
	    return true;
	if ((pageCount == 0) || (page < 1) || (page > pageCount))
	    return false;
	try {
  	    switch (parameterType) {
	    case SDDS_SHORT:
	        ledos.writeShort((short)longValue[page-1]);
	        break;
	    case SDDS_LONG: /* note that to be compatable with existing software we must write long values as an int */
	        ledos.writeInt((int)longValue[page-1]); 
	        break;
	    case SDDS_FLOAT:
	        ledos.writeFloat((float)doubleValue[page-1]);
	        break;
	    case SDDS_DOUBLE:
	        ledos.writeDouble(doubleValue[page-1]);
	        break;
	    case SDDS_STRING:
		if (stringValue[page-1] == null) {
		    ledos.writeInt(0);
	        } else {
		    ledos.writeInt(stringValue[page-1].length());
	            ledos.writeBytes(stringValue[page-1]);
	 	}
	        break;
	    case SDDS_CHARACTER: /* note that to be compatable with existing software we must use writeByte instead of writeChar */
	        ledos.writeByte((int)characterValue[page-1]);
	        break;
	    default:
	        return false;
	    }
	} catch (IOException e) {
	    return false;
	}
	return true;
    }

    /**
     * Get the values of the parameter for a chosen page
     *
     * @param page            the page of the parameter, must be greater then <code>0</code>
     * @param string_quotes   <code>true</code> if quotes are to be added around strings<br>
     *                        <code>false</code> if raw strings are to be outputed.
     * @return                Object containing parameter data
     */
    public Object getValue(int page, boolean string_quotes) {
	if ((pageCount == 0) || (page < 1) || (page > pageCount))
	    if (parameterFixedValue == null)
		return null;
	page--;
	try {
	    switch (parameterType) {
	    case SDDS_SHORT:
		if (parameterFixedValue != null)
		    return (new Short(parameterFixedValue));
		return (new Short((short)longValue[page]));
	    case SDDS_LONG:
		if (parameterFixedValue != null)
		    return (new Long(parameterFixedValue));
		return (new Long(longValue[page]));
	    case SDDS_FLOAT:
		if (parameterFixedValue != null)
		    return (new Float(parameterFixedValue));
		return (new Float((float)doubleValue[page]));
	    case SDDS_DOUBLE:
		if (parameterFixedValue != null)
		    return (new Double(parameterFixedValue));
		return (new Double(doubleValue[page]));
	    case SDDS_STRING:
		if (parameterFixedValue != null) {
		    if (string_quotes) {
			return SDDSUtil.prepareString(parameterFixedValue);
		    } else {
			return parameterFixedValue;
		    }
		}
		if (stringValue[page] == null)
		    stringValue[page] = "";
		if (string_quotes) {
		    return SDDSUtil.prepareString(stringValue[page]);
		} else {
		    return stringValue[page];
		}
	    case SDDS_CHARACTER:
		if (parameterFixedValue != null) {
		    if (parameterFixedValue.length() > 0) {
			return (new Character(parameterFixedValue.charAt(0))).toString();
		    } else {
			return "0";
		    }
		}
		if (Character.isLetterOrDigit(characterValue[page])) {
		    return (new Character(characterValue[page])).toString();
		} else {
		    String temp = Integer.toOctalString(characterValue[page]);
		    while (temp.length() < 3)
			temp = "0" + temp;
		    return ("\\" + temp);
		}
	    default:
		return null;
	    }
	} catch (Exception e) {
	    return null;
	}
    }
}
