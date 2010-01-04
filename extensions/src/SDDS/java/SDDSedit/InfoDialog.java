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

class InfoDialog extends JDialog {
    private JOptionPane optionPane;
    private JLabel label;

    public InfoDialog(Frame aFrame) {
        super(aFrame, true);

	setTitle("SDDS Editor Info");

	GridBagLayout gbl = new GridBagLayout();
	GridBagConstraints gbc = new GridBagConstraints();
	JPanel panel1 = new JPanel(gbl);

	label = new JLabel("<html>Programed by Robert Soliday<br><a href=\"mailto:soliday@aps.anl.gov\">soliday@aps.anl.gov</a></html>", JLabel.LEFT);
	label.setForeground(Color.black);
	gbc.ipadx = 10;
	gbc.fill = gbc.HORIZONTAL;
	gbc.weightx = 0.0;
	gbc.weighty = 0.0;
	gbc.gridwidth = gbc.REMAINDER;
	gbl.setConstraints(label, gbc);
	panel1.add(label);

        Object[] array = {panel1};


        final String btnString1 = "I love this program!!!";
        Object[] options = {btnString1};

        optionPane = new JOptionPane(array, 
                                    JOptionPane.INFORMATION_MESSAGE,
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
                    }
                }
            }
        });
    }
}
