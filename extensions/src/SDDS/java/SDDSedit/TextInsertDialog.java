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
import SDDS.java.SDDS.*;

class TextInsertDialog extends JDialog {
    private String text = null;
    private JOptionPane optionPane;
    private JLabel label;
    private JTextField field;

    public String getText() {
	return text;
    }

    public void setTitleInfo(String title) {
	setTitle(title);
    }
    public void setEntryLabel(String text) {
	label.setText(text);
    }
    public void setEntryText(String text) {
	field.setText(text);
    }
    public TextInsertDialog(Frame aFrame) {
        super(aFrame, true);

	setTitle("");

	GridBagLayout gbl = new GridBagLayout();
	GridBagConstraints gbc = new GridBagConstraints();
	JPanel panel1 = new JPanel(gbl);

	label = new JLabel("", JLabel.LEFT);
	label.setForeground(Color.black);
	gbc.ipadx = 10;
	gbc.fill = gbc.HORIZONTAL;
	gbc.weightx = 0.0;
	gbc.weighty = 0.0;
	gbc.gridwidth = 1;
	gbl.setConstraints(label, gbc);
	panel1.add(label);
        field = new JTextField(10);
	gbc.weightx = 1.0;
	gbc.gridwidth = gbc.REMAINDER;
	gbl.setConstraints(field, gbc);
	panel1.add(field);

        Object[] array = {panel1};


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

        field.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent e) {
                optionPane.setValue(btnString1);
            }
        });

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
			text = field.getText();
			setVisible(false);
                    } else { // user closed dialog or clicked cancel
                        text = null;
                        setVisible(false);
                    }
                }
            }
        });
    }
}
