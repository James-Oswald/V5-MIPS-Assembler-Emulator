
import java.awt.Color;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.image.BufferedImage;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JLabel;
import java.util.ArrayList;

public class Display extends JFrame
{
	private final static int REGWIDTH = 150, REGHEIGHT = 20, REGLEFT = 10, REGTOP = 10, COLMAX = 9;
	private int[] registerValues, lastState;
	private String[] registerNames;
	private JLabel[] registerLabels;
	private Color[] registerColors;
	
	private final static int MLWIDTH = 100, MLHEIGHT = 10, MLLEFT = 10, MLTOP = 200;
	protected JLabel memLoc;
	
	private final static int MEMWIDTH = 20, MEMHEIGHT = 15, MEMLEFT = 10, MEMTOP = 220, ROWMAX = 20, MEMSIZE = 400;
	private byte[] memValues;
	private JLabel[] memLables;
	
	private final static int DISPWIDTH = 256, DISPHEIGHT = 144, DISPLEFT = 500, DISPTOP = 500;
	private BufferedImage displayData;
	private JPanel displayPanel;
	
	private final static int OUTWIDTH = 256, OUTHEIGHT = 144, OUTLEFT = 500, OUTTOP = DISPTOP + DISPHEIGHT;
	private ArrayList<String> outData;
	private JLabel outPanel;
	
	public Display(String[] registerNames)
	{
		super();
		this.registerNames = registerNames;
		registerValues = new int[registerNames.length];
		registerLabels = new JLabel[registerNames.length];
		registerColors = new Color[registerNames.length];
		lastState = new int[registerNames.length];
		JPanel content = new JPanel();
		content.setLayout(null);
		for(int i = 0; i < registerNames.length; i++)
		{
			registerLabels[i] = new JLabel(formatRegister(i));
			int x = (int)Math.floor(i / COLMAX) * REGWIDTH + REGLEFT;
			int y = (REGHEIGHT * i) % (REGHEIGHT * COLMAX) + REGTOP;
			registerLabels[i].setBounds(x, y, REGWIDTH, REGHEIGHT);
			registerColors[i] = Color.getHSBColor((float)((double)i/(registerNames.length + 3)), 1f,0.9f);
			registerLabels[i].setForeground(Color.white);
			registerLabels[i].setBackground(registerColors[i]);
			registerLabels[i].setOpaque(true);
			content.add(registerLabels[i]);
			
		}
		memLoc = new JLabel("0");
		memLoc.setBounds(MLLEFT, MLTOP, MLWIDTH, MLHEIGHT);
		content.add(memLoc);
		memValues = new byte[MEMSIZE];
		memLables = new JLabel[MEMSIZE];
		for(int i = 0; i < MEMSIZE; i++)
		{
			memLables[i] = new JLabel(formatMemory(i));
			int y = (int)Math.floor(i / ROWMAX) * MEMHEIGHT + MEMTOP;
			int x = (MEMWIDTH * i) % (MEMWIDTH * ROWMAX) + MEMLEFT;
			memLables[i].setBounds(x, y, MEMWIDTH, MEMHEIGHT);
			MouseListener listener = new MouseListener()
			{
				public void mouseClicked(MouseEvent e){}
				public void mouseEntered(MouseEvent e){memLoc.setText(" " + getAddress((JLabel)e.getSource()));}
				public void mouseExited(MouseEvent e){}
				public void mousePressed(MouseEvent e){}
				public void mouseReleased(MouseEvent e){}
			};
			memLables[i].addMouseListener(listener);
			memLables[i].setOpaque(true);
			content.add(memLables[i]);
		}
		displayData = new BufferedImage(DISPWIDTH, DISPHEIGHT, BufferedImage.TYPE_INT_RGB);
		displayPanel = new JPanel()
		{
			@Override public void paintComponent(Graphics g)
			{
				super.paintComponent(g);
				g.drawImage((Image)displayData, 0, 0, null);
			}
		};
		displayPanel.setBounds(DISPLEFT, DISPHEIGHT, DISPLEFT, DISPTOP);
		content.add(displayPanel);
		outData = new ArrayList<String>();
		outPanel = new JLabel("");
		outPanel.setBounds.setBounds(OUTLEFT, OUTHEIGHT, OUTLEFT, OUTTOP);
		content.add(outPanel);
		this.setContentPane(content);
		this.setSize(800, 600);
		this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		this.setVisible(true);
	}
	
	private final static String[] MIPSNames =
	{
		"ze", "at", "v0", "v1", "a0", "a1", "a2", "a3",
		"t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
		"s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
		"t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra", "pc"
	};
	
	public Display()
	{
		this(MIPSNames);
	}
	
	private String formatRegister(int index)
	{
		return registerNames[index] + ": 0x" + String.format("%08x", registerValues[index]) + ": " + registerValues[index];
	}
	
	private String formatMemory(int index)
	{
		return String.format("%02x", memValues[index]);
	}
	
	private int getAddress(JLabel l)
	{	
		return (l.getX()-MEMLEFT)/MEMWIDTH+((l.getY()-MEMTOP)/MEMHEIGHT) * ROWMAX;
	}
	
	private int linearSearch(int[] a, int t)
	{
		for(int i = 0; i < a.length; i++)
			if(a[i] == t)
				return i;
		return -1;
	}
	
	public void setRegisters(int[] values)
	{
		registerValues = values;
		for(int i = 0; i < registerLabels.length;i++)
			registerLabels[i].setText(formatRegister(i));
	}
	
	//Bulky
	/*
	private int linearSearch(int[] a, int t)
	{
		for(int i = 0; i < a.length; i++)
			if(a[i] == t)
				return i;
		return -1;
	}
	
	public void setMemory(byte[] values)
	{
		memValues = values;
		for(int i = 0; i < memLables.length; i++)
		{
			int index = linearSearch(registerValues, i);
			if(index != -1)memLables[i].setBackground(registerColors[index]);
			else memLables[i].setBackground(Color.white);
			memLables[i].setText(formatMemory(i));
		}
	}
	*/

	public void setMemory(byte[] values)
	{
		for(int i = 0; i < memLables.length; i++)
		{
			if(values[i] != memValues[i])
			{
				memValues[i] = values[i];
				memLables[i].setText(formatMemory(i));
			}
		}
		for(int i = 0; i < registerLabels.length; i++)
		{
			if(lastState[i] < memLables.length)memLables[lastState[i]].setBackground(Color.white);
			if(registerValues[i] < memLables.length)memLables[registerValues[i]].setBackground(registerColors[i]);
		}
		lastState = registerValues;
	}
	
	public void setDisplay(int[] values)
	{
		for(int i = 0; i < values.length; i++)
		{
			displayData.setRGB((i + 1) % DISPWIDTH, (int)Math.floor(i / (double)DISPWIDTH), values[i]);
		}
		displayPanel.repaint();
	}
	
	public void addOutput(String output)
	{
		outData.add(output);
		String outText = "";
		int newLineCount = 0;
		for(int i = outData.size() - 1; i >= 0 && newLineCount <= 5; i--)
		{
			for(int j = 0; j < outData.get(i).length(); j++)
			{
				if(outData[i] )
			}
			
		}
	}
	
	public static void main(String[] args)
	{
		new Display();
	}
}