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
import SDDS.java.SDDS.*;

class SearchDialog extends JDialog {
    private String[] names = null;
    private JOptionPane optionPane;
    private JTextField[] fields = null;
    private JRadioButton[] labels = null;
    private JPanel panel1=null;
    private JScrollPane scrollPane=null;
    private GridBagLayout gbl=null;
    private GridBagConstraints gbc=null;
    private ButtonGroup typeButtonGroup=null;
    private boolean canceled=false;

    public String getName() {
	if ((canceled == true) || (names == null)) {
	    return null;
	}
	int length,i;
	length = Array.getLength(names);
	for (i=0;i<length;i++) {
	    if (labels[i].isSelected()) {
		return names[i];
	    }
	}
	return null;
    }

    public String getPattern() {
	if ((canceled == true) || (names == null)) {
	    return null;
	}
	int length,i;
	length = Array.getLength(names);
	for (i=0;i<length;i++) {
	    if (labels[i].isSelected()) {
		return fields[i].getText();
	    }
	}
	return null;
    }

    public void setNames(String[] n) {
	if (n != null) {
	    int length, i;
	    length = Array.getLength(n);
	    if (names == null)
		names = new String[length];
	    else
		names = (String[])SDDSUtil.resize(names, length);
	    for (i=0;i<length;i++) {
		names[i] = n[i];
	    }
	}
    }

    public void addColumns(String[] n) {
	canceled = false;
	if (n == null)
	    return;
	int length, i;
	length = Array.getLength(n);
	boolean same = false;
	if (names != null) {
	    if (length == Array.getLength(names)) {
		same = true;
		for (i=0;i<length;i++) {
		    if (!names[i].equals(n[i])) {
			same = false;
			break;
		    }
		}
	    }
	}
	if (same)
	    return;
	int height;
	if (length > 13) {
	    height = 352;
	} else {
	    height = length * 25 + 2;
	}
        scrollPane.setPreferredSize(new Dimension(300, height));
        scrollPane.setMinimumSize(new Dimension(300, height));

	panel1.removeAll();
	names = n;
	labels = new JRadioButton[length];
	fields = new JTextField[length];
	for (i=0;i<length;i++) {
	    labels[i] = new JRadioButton(n[i]);
	    typeButtonGroup.add(labels[i]);
	    if (i==0)
		labels[i].setSelected(true);
	    gbc.ipadx = 10;
	    gbc.fill = gbc.HORIZONTAL;
	    gbc.weightx = 0.0;
	    gbc.weighty = 0.0;
	    gbc.gridwidth = 1;
	    gbl.setConstraints(labels[i], gbc);
	    panel1.add(labels[i]);
	    fields[i] = new JTextField(10);
	    gbc.weightx = 1.0;
	    gbc.gridwidth = gbc.REMAINDER;
	    gbl.setConstraints(fields[i], gbc);
	    panel1.add(fields[i]);
	}
    }


    public SearchDialog(Frame aFrame) {
        super(aFrame, true);
	setTitle("Search");

	
	gbl = new GridBagLayout();
	gbc = new GridBagConstraints();
	panel1 = new JPanel(gbl);
	scrollPane = new JScrollPane(panel1);
	typeButtonGroup = new ButtonGroup();
        Object[] array = {scrollPane};


        final String btnString1 = "OK";
        final String btnString2 = "Cancel";
        Object[] options = {btnString1, btnString2};

        optionPane = new JOptionPane(array, 
                                    JOptionPane.QUESTION_MESSAGE,
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
        fields[0].addActionListener(new ActionListener() {
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

                    if (value.equals(btnString1)) {
			setVisible(false);
		    } else { // user closed dialog or clicked cancel
			canceled = true;
                        setVisible(false);
                    }
                }
            }
        });
    }
}
