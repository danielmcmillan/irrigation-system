1. Generate BOM from Eeschema.

   - Choose any format (only intermediate xml file is used).
   - Xml file is written to the project folder.

2. Generate Footprint Position Files from Pcbnew.

   - Set Output directory to "out".
   - Format: CSV, Units: Millimeters, Files: Single file for board.
   - \*-all-pos.csv file is written to the out folder.

3. Generate gerber files.

   - Layers: F.Cu, B.Cu, F.Paste, F.SilkS, B.SilkS, F.Mask, B.Mask, Edge.Cuts.
   - Check Use Protel filename extensions.
   - Check Subtract soldermask from silkscreen.
   - More details: https://support.jlcpcb.com/article/149-how-to-generate-gerber-and-drill-files-in-kicad
   - _.gbl, _.gbs, _.gbo, _.gm1, _.gtl, _.gto, _.gtp and _.gts files are written to the out folder.

4. Generate drill files.

   - Choose Use alternate drill mode as the Oval Holes Drill Mode.
   - Drill Origin: Absolute, Drill Units: millimeters, Zeros Format: Decimal format.
   - Click both "Generate Drill File" and "Generate Map File".
   - _.drl and _.gbr files are written to the out folder.

5. Run script from https://github.com/matthewlai/JLCKicadTools.

   - Install dependencies: `pip install -r requirements.txt`
   - This line needs adding to the "jlc_kicad_tools/cpl_rotations_db.csv" file: `"^Texas_Instruments-TPS54331DR-Level_A",270`
   - Install: `python3 setup.py install --user`
   - Run in the KiCad project directory: `jlc-kicad-tools . -o ./out`
   - _\_bom_jlc.csv and _\_cpl_jlc.csv files are written to the out folder.

6. Archive contents of out folder into a zip file.

7. Order from jlcpcb.

   - Upload the zip file when asked for gerber files.
   - Upload the \*\_bom_jlc.csv file when asked for BOM file.
   - Upload the \*\_cpl_jlc.csv file when asked for CPL file (not the all-pos.csv).
