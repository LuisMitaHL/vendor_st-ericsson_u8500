/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *
 * This code is part of the NMF Toolset.
 *
 * The NMF Toolset is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * The NMF Toolset is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the NMF Toolset. If not, see <http://www.gnu.org/licenses/>.
 *
 */
package tools.common;

public class util {
    
    public static long StringToUnsigned(String value) throws NumberFormatException {
        if(value.startsWith("0x") || value.startsWith("0X")) 
            return Long.parseLong(value.substring(2), 16); 
        else if(value.equals("0"))
            return 0;
        else if(value.startsWith("0"))
            return Long.parseLong(value.substring(1), 8); 
        else if(Character.isDigit(value.charAt(0)))
            return Long.parseLong(value, 10); 
        else 
            throw new NumberFormatException();
    } 

    public static long StringToDecimal(String value) throws NumberFormatException {
        // Check sign
        long sign = 1;
        if(value.charAt(0) == '-') {
            sign = -1;
            value = value.substring(1);
        }
        if(value.charAt(0) == '+') {
            value = value.substring(1);
        }
        
        return sign * StringToUnsigned(value);
    } 
        
    public static String getLastName(String fullyname) 
    {
        String smallname;
        
        if(fullyname.lastIndexOf('.') == -1) {
            //pack = "";
            smallname = fullyname;
        } else {
            smallname = fullyname.substring(fullyname.lastIndexOf('.') + 1);
        }       

        return smallname;
    }
}