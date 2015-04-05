/* Copyright 2010 Kenneth 'Impaler' Ferland

 This file is part of Khazad.

 Khazad is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Khazad is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Khazad.  If not, see <http://www.gnu.org/licenses/> */

package PathFinding;

import Map.Direction;
import Map.MapCoordinate;

/**
 * PathWalkers are objects that organizes the reading and following of Paths,
 * each instance of a PathWalker is spawned from a specifice Path and caries
 * it's own internal location along that Path, so multiple PathWalkers can
 * exist and read from a single Path. Next methods advance the Walker down the
 * Path while Peek methods do not.
 *
 * @author Impaler
 */
public interface PathWalker {

	public MapCoordinate nextCoordinate();

	public Direction nextDirection();

	public MapCoordinate peekCoordinate();

	public Direction peekDirection();

	public int getCurrentStep();

	void reset();
}
