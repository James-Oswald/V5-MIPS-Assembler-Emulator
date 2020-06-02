
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JLabel;
import java.awt.Graphics;
import java.awt.Color;

public class Display extends JFrame
{
	private final static int REGWIDTH = 150, REGHEIGHT = 20, REGLEFT = 0, REGTOP = 10, COLMAX = 8;
	
	private final static String[] MIPSNames =
	{
		"ze", "at", "v0", "v1", "a0", "a1", "a2", "a3",
		"t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
		"s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
		"t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"
	};

	private int[] registerValues;
	private String[] registerNames;
	private JLabel[] registerLabels;
	
	public Display(String[] registerNames)
	{
		super();
		this.registerNames = registerNames;
		registerValues = new int[registerNames.length];
		registerLabels = new JLabel[registerNames.length];
		JPanel content = new JPanel();
		content.setLayout(null);
		for(int i = 0; i < registerNames.length; i++)
		{
			registerLabels[i] = new JLabel(registerToString(i));
			int x = (int)Math.floor(i / COLMAX) * REGWIDTH + REGLEFT;
			int y = (REGHEIGHT * i) % (REGHEIGHT * COLMAX) + REGTOP;
			registerLabels[i].setBounds(x, y, REGWIDTH, REGHEIGHT);
			content.add(registerLabels[i]);
		}
		this.setContentPane(content);
		this.setSize(800, 600);
		this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		this.setVisible(true);
	}
	
	private String registerToString(int index)
	{
		return registerNames[index] + ": 0x" + String.format("%08x", registerValues[index]) + ": " + registerValues[index];
	}
	
	public static void main(String[] args)
	{
		new Display(MIPSNames);
	}
}