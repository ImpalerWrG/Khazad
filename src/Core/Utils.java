/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package Core;

/**
 *
 * @author Dallas
 */
public class Utils {	
	public static String padLeadingZero(long number)
	{
		if (number < 10)
		{
			return "0" + Long.toString(number);
		}
		else
		{
			return Long.toString(number);
		}
	}
}
