/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package Map;

/**
 *
 * @author Impaler
 */
public class FaceShape
{
    CubeShape CubeComponent;
    Direction FaceDirection;
	
    public FaceShape() {
        CubeComponent = new CubeShape(CubeShape.BELOW_CUBE_HEIGHT);
        FaceDirection = Direction.DIRECTION_DESTINATION;
    }

    public FaceShape(CubeShape ShapeType, Direction DirectionType) {
        CubeComponent = ShapeType;
        FaceDirection = DirectionType;
    }

    boolean equals(FaceShape ArgumentShape) {
        return CubeComponent == ArgumentShape.CubeComponent && FaceDirection == ArgumentShape.FaceDirection;
    }

    boolean notequal(FaceShape ArgumentShape) {
        return CubeComponent != ArgumentShape.CubeComponent || FaceDirection != ArgumentShape.FaceDirection;
    }

	int Key() {
		return (CubeComponent.Data << 16) + FaceDirection.Data;
	}
}
