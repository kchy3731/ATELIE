current camera move: wasdqe, mouse drag to change perspective

## the command should be typed on the app window, not terminal (but terminal will print the command buffer), hit enter to execute command

## Active Opcode Structure Layout 

Transform command follows this rule framework:

### `Digit 1`: Category
- `0` : Immediately reset all transformations manually.
- `1` : Execute Spatial Transform
- `2` : Set target Mesh Model (`Digit 2` immediately maps to 0 / 1), right now only 0 (triangle) and 1 (cube)
- `FUTURE FUNCTIONS`

### `Digit 2`: Operation Type
- `1` : Scale Factor
- `2` : Translate Coordinate (Move)
- `3` : Rotation

### `Digit 3`: Axis Evaluation Targeting
- `1`: Target X-Axis
- `2`: Target Y-Axis
- `3`: Target Z-Axis
- `4`: All (Uniform Target. Standardly applied for uniform scales).

### `Digit 4`: Directional Math Toggle (Your feature upgrade!)
- `1`: **Negative Modifier:** Operates towards the negatives (e.g. rotates backward, translates opposite, or triggers fractional math to functionally **scale down**!).
- `2`: **Positive Modifier:** Standard bounds (translates positive, triggers scale up naturally, sets angular rotation forward).

### `Digit 5+`: Data Value Input
- You can trail any immediate payload values sequentially after Digit 4. (For example, `1` trailed to represent 1 coordinate degree, or `45` to represent 45 coordinates).
