/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

package SDDS.java.SDDSedit;

import javax.swing.*;
import java.beans.*;
import java.awt.*;
import java.awt.event.*;
import java.lang.reflect.*;
import java.lang.*;
import SDDS.java.SDDS.*;

class MatchList extends JFrame {
    private String[] elements = null;
    private JOptionPane optionPane;
    private JLabel label;
    private JList list;
    private JPanel panel1=null;
    private GridBagLayout gbl=null;
    private GridBagConstraints gbc=null;
    private int listCount=0;

    public void setElements(String[] ele) {
	if (ele != null) {
	    int i;
	    listCount = Array.getLength(ele);
	    if (elements == null)
		elements = new String[listCount];
	    else
		elements = (String[])SDDSUtil.resize(elements, listCount);
	    for (i=0;i<listCount;i++) {
		elements[i] = ele[i];
	    }
	    panel1.removeAll();
	    
	    label = new JLabel("Element@Cell(row,column)", JLabel.LEFT);
	    label.setForeground(Color.black);
	    gbc.ipadx = 10;
	    gbc.fill = gbc.HORIZONTAL;
	    gbc.weightx = 0.0;
	    gbc.weighty = 0.0;
	    gbc.gridwidth = gbc.REMAINDER;
	    gbl.setConstraints(label, gbc);
	    panel1.add(label);
	    list = new JList(elements);
	    gbc.weightx = 1.0;
	    gbc.gridwidth = gbc.REMAINDER;
	    JScrollPane listScrollPane = new JScrollPane(list);
	    listScrollPane.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
	    gbl.setConstraints(listScrollPane, gbc);
	    panel1.add(listScrollPane);
	}
    }

    public MatchList() {
        //super(aFrame, true);

	setTitle("Match List");

	gbl = new GridBagLayout();
	gbc = new GridBagConstraints();
	panel1 = new JPanel(gbl);

        Object[] array = {panel1};


        final String btnString1 = "Close";
        final String btnString2 = "Select";
        final String btnString3 = "Select All";
        final String btnString4 = "Delete Rows";
        Object[] options = {btnString1, btnString2, btnString3, btnString4};

        optionPane = new JOptionPane(array, 
                                    JOptionPane.PLAIN_MESSAGE,
                                    JOptionPane.DEFAULT_OPTION,
                                    null,
                                    options,
                                    options[0]);
        setContentPane(optionPane);
        setDefaultCloseOperation(DO_NOTHING_ON_CLOSE);
        addWindowListener(new WindowAdapter() {
                public void windowClosing(WindowEvent we) {
                /*
                 * Instead of directly closing the window,
                 * we're going to change the JOptionPane's
                 * value property.
                 */
                    optionPane.setValue(new Integer(
                                        JOptionPane.CLOSED_OPTION));
            }
        });

	/*
        list.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                optionPane.setValue(btnString1);
            }
        });
	*/
        optionPane.addPropertyChangeListener(new PropertyChangeListener() {
            public void propertyChange(PropertyChangeEvent e) {
                String prop = e.getPropertyName();

                if (isVisible() 
                 && (e.getSource() == optionPane)
                 && (prop.equals(JOptionPane.VALUE_PROPERTY) ||
                     prop.equals(JOptionPane.INPUT_VALUE_PROPERTY))) {
                    Object value = optionPane.getValue();

                    if (value == JOptionPane.UNINITIALIZED_VALUE) {
                        //ignore reset
                        return;
                    }

                    // Reset the JOptionPane's value.
                    // If you don't do this, then if the user
                    // presses the same button next time, no
                    // property change event will be fired.
                    optionPane.setValue(
                            JOptionPane.UNINITIALIZED_VALUE);

                    if (value.equals(btnString3)) { 
			list.setSelectionInterval(0,listCount-1);
		    }
                    if (value.equals(btnString1)) { 
			// user closed dialog or clicked cancel
			setVisible(false);
		    } else if (value.equals(btnString2) || value.equals(btnString3)) {
			Object[] selectedValues = null;
			int length, i, indexStart, indexEnd, row, col;
			boolean firstSelectedElement = true;
			selectedValues = list.getSelectedValues();
			length = Array.getLength(selectedValues);
			sddsEdit.clearColumnSelections();
			for ( i=0 ; i < length ; i++ ) {
			    indexStart = ((String)selectedValues[i]).lastIndexOf('@');
			    indexEnd = ((String)selectedValues[i]).lastIndexOf(',');
			    row = Integer.valueOf(((String)selectedValues[i]).substring(indexStart+1,indexEnd)).intValue() - 1;
			    col = Integer.valueOf(((String)selectedValues[i]).substring(indexEnd+1,((String)selectedValues[i]).length())).intValue() - 1;
			    sddsEdit.selectColumnElement(row, col);
			    if (firstSelectedElement) {
				sddsEdit.scrollToColumnElement(row, col);
				firstSelectedElement = false;
			    }
			}
                    } else if (value.equals(btnString4)) {
			Object[] selectedValues = null;
			int length, i, indexStart, indexEnd, row;
			selectedValues = list.getSelectedValues();
			length = Array.getLength(selectedValues);
			for ( i=length-1 ; i >= 0 ; i-- ) {
			    indexStart = ((String)selectedValues[i]).lastIndexOf('@');
			    indexEnd = ((String)selectedValues[i]).lastIndexOf(',');
			    row = Integer.valueOf(((String)selectedValues[i]).substring(indexStart+1,indexEnd)).intValue() - 1;
			    sddsEdit.deleteColumnRow(row);
			}
			if (length > 0) {
			    //setVisible(false);
			    sddsEdit.autoSearch();
			}
                    }
                }
            }
        });
    }
}
