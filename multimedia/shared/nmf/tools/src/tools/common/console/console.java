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
package tools.common.console;

/**
 * Provides console color access.
 */
public interface console {

    /**
     * Set console foreground color.
     * 
     * @param   color   Set this color.
     */
    public void setForegroundColor(foregroundcolor color);

    /**
     * Set console background color.
     * 
     * @param   color   Set this color.
     */
    public void setBackgroundColor(backgroundcolor color);

    /**
     * Reset console backgound and foreground colors to initial values.
     */
    public void resetColors();

}
