/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Interface;

import Map.MapCoordinate;
/**
 *
 * @author Impaler
 */
public class VolumeSelection {
	
	public MapCoordinate OriginLocation = new MapCoordinate();
    public MapCoordinate TerminalLocation = new MapCoordinate();

	public boolean Dirty;
	
	public VolumeSelection() {
		Dirty = true;
	}

	public VolumeSelection(MapCoordinate SelectionOrigin, MapCoordinate SelectionTerminus) {
		SetSize(SelectionOrigin,SelectionTerminus);
		Dirty = true;
	}
	
	public final void SetSize(MapCoordinate SelectionOrigin, MapCoordinate SelectionTerminus) {
		
		int maxX = Math.max(SelectionTerminus.X, SelectionOrigin.X);
		int maxY = Math.max(SelectionTerminus.Y, SelectionOrigin.Y);
		int maxZ = Math.max(SelectionTerminus.Z, SelectionOrigin.Z);

		int minX = Math.min(SelectionTerminus.X, SelectionOrigin.X);
		int minY = Math.min(SelectionTerminus.Y, SelectionOrigin.Y);
		int minZ = Math.min(SelectionTerminus.Z, SelectionOrigin.Z);

		OriginLocation.Set(minX, minY, minZ);
		TerminalLocation.Set(maxX, maxY, maxZ);
		
		Dirty = true;
	}
}
