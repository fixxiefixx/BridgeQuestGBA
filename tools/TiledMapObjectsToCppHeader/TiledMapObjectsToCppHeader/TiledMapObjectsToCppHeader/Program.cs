using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.Text;
using TiledSharp;

namespace TiledMapObjectsToCppHeader
{
    class Program
    {

        static TmxTilesetTile GetTileFromGid(TmxMap map, int gid)
        {
            for (int i = 0; i < map.Tilesets.Count; i++)
            {
                TmxTileset testSet = map.Tilesets[i];
                if (testSet.FirstGid <= gid && (testSet.FirstGid + testSet.TileCount) > gid)
                {
                    return testSet.Tiles[gid - testSet.FirstGid];
                }
            }
            return null;
        }

        static int RoundToInt(double value)
        {
            if (value >= 0)
            {
                return (int)(Math.Round(value) + 0.00001);
            }
            else
            {
                return (int)(Math.Round(value) - 0.00001);
            }
        }

        static string TmxObjectToRectStr(TmxObject tmxObject, TmxObjectPoint topLeftCorner, CultureInfo ci)
        {
            return "gt::simple_fixed_rect(" + (tmxObject.X + topLeftCorner.X).ToString(ci) + "," + (tmxObject.Y + topLeftCorner.Y).ToString(ci) + "," +
                        RoundToInt(tmxObject.Width).ToString() + "," + RoundToInt(tmxObject.Height).ToString() + ")";
        }

        static string TmxObjectToBnRectStr(TmxObject tmxObject, TmxObjectPoint topLeftCorner, CultureInfo ci)
        {
            return "bn::fixed_rect(" + (tmxObject.X + topLeftCorner.X+tmxObject.Width/2).ToString(ci) + "," + (tmxObject.Y + topLeftCorner.Y+tmxObject.Height/2).ToString(ci) + "," +
                        RoundToInt(tmxObject.Width).ToString() + "," + RoundToInt(tmxObject.Height).ToString() + ")";
        }

        static string TmxObjectToFixedPointStr(TmxObject tmxObject, TmxObjectPoint topLeftCorner, CultureInfo ci)
        {
            return "bn::fixed_point(" + (tmxObject.X + topLeftCorner.X).ToString(ci) + "," + (tmxObject.Y + topLeftCorner.Y).ToString(ci) + ")";
        }

        static string CppEscStr(string str)
        {
            return str.Replace("\n", "\\n").Replace("\"", "\\\"");
        }

        static void addToSetIfNotContains(HashSet<string> set, string value)
        {
            if (String.IsNullOrEmpty(value))
                return;

            if (!set.Contains(value))
                set.Add(value);
        }

        static void Main(string[] args)
        {
            if (args.Length < 1)
            {
                Console.WriteLine("No argument");
                return;
            }

            CultureInfo ci = CultureInfo.CreateSpecificCulture("en-US");

            string mapDir = args[0];
            string outputFileName = Path.Combine(mapDir, "maps.h");
            string defFileName = Path.Combine(mapDir, "definitions.h");

            string[] files = Directory.GetFiles(mapDir);
            Console.WriteLine("Output file: " + outputFileName);

            HashSet<string> defSet = new HashSet<string>();
            addToSetIfNotContains(defSet, "null");
            addToSetIfNotContains(defSet,"bat");
            addToSetIfNotContains(defSet, "knight");
            addToSetIfNotContains(defSet, "chest");
            addToSetIfNotContains(defSet, "tree_boss_minion");
            addToSetIfNotContains(defSet, "heart");
            addToSetIfNotContains(defSet, "heart_container");

            using (StreamWriter sw = new StreamWriter(outputFileName, false, Encoding.UTF8))
            {

                Dictionary<string, int> objNameIds = new Dictionary<string, int>();

                sw.WriteLine("#ifndef MAPS_H");
                sw.WriteLine("#define MAPS_H");
                sw.WriteLine();
                sw.WriteLine("#include \"bn_core.h\"");
                sw.WriteLine("#include \"bn_fixed_rect.h\"");
                sw.WriteLine("#include \"gt_simple_fixed_rect.h\"");
                sw.WriteLine("#include \"gt_sign.h\"");
                sw.WriteLine("#include \"gt_warp.h\"");
                sw.WriteLine("#include \"gt_map_info.h\"");
                sw.WriteLine("#include \"gt_objinfo.h\"");
                sw.WriteLine("#include \"definitions.h\"");
                sw.WriteLine();
                List<string> mapNamesWritten = new List<string>();

                foreach (string mapFileName in files)
                {
                    if (!mapFileName.EndsWith(".tmx", StringComparison.CurrentCultureIgnoreCase))
                        continue;

                    string inputFileWithoutPath = Path.GetFileName(mapFileName);
                    string mapname = inputFileWithoutPath.Substring(0, inputFileWithoutPath.Length - 4);
                    mapNamesWritten.Add(mapname);

                    sw.WriteLine("#include \"bn_regular_bg_items_"+mapname+".h\"");

                    TmxMap map = new TmxMap(mapFileName);
                    TmxLayer objectLayer = null;

                    foreach (TmxLayer layer in map.Layers)
                    {
                        if (layer.Name == "Objects")
                        {
                            objectLayer = layer;
                            break;
                        }
                    }

                    if (objectLayer == null)
                    {
                        Console.WriteLine("Missing \"Objects\" layer.");
                        return;
                    }

                    TmxObjectGroup areaGroup = null;
                    TmxObjectGroup signGroup = null;
                    TmxObjectGroup warpGroup = null;
                    TmxObjectGroup objInfoGroup = null;

                    foreach (TmxObjectGroup objGroup in map.ObjectGroups)
                    {
                        if (objGroup.Name == "Areas")
                        {
                            areaGroup = objGroup;
                        }
                        else if (objGroup.Name == "Signs")
                        {
                            signGroup = objGroup;
                        }
                        else if (objGroup.Name == "Warps")
                        {
                            warpGroup = objGroup;
                        }
                        else if (objGroup.Name == "ObjData")
                        {
                            objInfoGroup = objGroup;
                        }
                    }

                    TmxObjectPoint topLeftCorner = new TmxObjectPoint((map.Width * map.TileWidth) / -2, (map.Height * map.TileHeight) / -2);

                    
                    


                    sw.WriteLine(String.Format("#define {0}_mapobjects_width {1}", mapname, map.Width));
                    sw.WriteLine(String.Format("#define {0}_mapobjects_height {1}", mapname, map.Height));
                    sw.WriteLine(String.Format("#define {0}_area_count {1}", mapname, areaGroup.Objects.Count));
                    sw.WriteLine(String.Format("#define {0}_sign_count {1}", mapname, signGroup.Objects.Count));
                    sw.WriteLine(String.Format("#define {0}_warp_count {1}", mapname, warpGroup.Objects.Count));
                    sw.WriteLine(String.Format("#define {0}_objinfo_count {1}", mapname, objInfoGroup.Objects.Count));

                    //Write areas
                    sw.WriteLine("const gt::simple_fixed_rect " + mapname + "_areas[] = {");
                    for (int i = 0; i < areaGroup.Objects.Count; i++)
                    {
                        TmxObject tmxObject = areaGroup.Objects[i];
                        /*sw.Write("bn::fixed_rect(" + (tmxObject.X+topLeftCorner.X+tmxObject.Width/2).ToString(ci) + "," + (tmxObject.Y+topLeftCorner.Y+tmxObject.Height/2).ToString(ci) + "," +
                            RoundToInt(tmxObject.Width).ToString() + "," + RoundToInt(tmxObject.Height).ToString() + ")");*/
                        sw.Write(TmxObjectToRectStr(tmxObject, topLeftCorner, ci));
                        if (i < areaGroup.Objects.Count - 1)
                        {
                            sw.Write(",");
                        }
                        sw.WriteLine();
                    }
                    sw.WriteLine("};");

                    //Write signs
                    sw.WriteLine("const gt::sign "+mapname+"_signs[] = {");
                    for (int i = 0; i < signGroup.Objects.Count; i++)
                    {
                        TmxObject tmxObject = signGroup.Objects[i];
                        String signText = CppEscStr(tmxObject.Properties["text"]);
                        sw.Write("gt::sign(" + TmxObjectToRectStr(tmxObject, topLeftCorner, ci) + ", \"" + signText + "\")");

                        if (i < signGroup.Objects.Count - 1)
                        {
                            sw.Write(",");
                        }
                        sw.WriteLine();
                    }
                    if (signGroup.Objects.Count == 0)
                        sw.WriteLine("gt::sign(gt::simple_fixed_rect(0,0,0,0), \"\")");
                    sw.WriteLine("};");

                    //Write warps
                    sw.WriteLine("const gt::warp "+mapname+"_warps[] = {");
                    for (int i = 0; i < warpGroup.Objects.Count; i++)
                    {
                        TmxObject tmxObject = warpGroup.Objects[i];
                        String dest_map = tmxObject.Properties["dest_map"];
                        int dest_rotation = int.Parse(tmxObject.Properties["dest_rotation"]);
                        String dest_warp = tmxObject.Properties["dest_warp"];
                        String name = tmxObject.Name;
                        addToSetIfNotContains(defSet, name);
                        addToSetIfNotContains(defSet, dest_map);
                        addToSetIfNotContains(defSet, dest_warp);
                        sw.Write("gt::warp(Def::" + name + ",Def::" + dest_map + ",Def::"+dest_warp+"," + TmxObjectToFixedPointStr(tmxObject, topLeftCorner, ci) + "," + dest_rotation.ToString() + ")");
                        if (i < warpGroup.Objects.Count - 1)
                        {
                            sw.Write(",");
                        }
                        sw.WriteLine();
                    }
                    sw.WriteLine("};");

                    //Write objinfos
                    sw.WriteLine("const gt::objinfo " + mapname + "_objinfos[] = {");
                    for(int i = 0;i < objInfoGroup.Objects.Count;i++)
                    {
                        TmxObject tmxObject = objInfoGroup.Objects[i];
                        String name = String.IsNullOrEmpty(tmxObject.Name) ? "null" : tmxObject.Name;
                        String type = String.IsNullOrEmpty(tmxObject.Type)?"null":tmxObject.Type;
                        String item;
                        if (!tmxObject.Properties.TryGetValue("item", out item))
                            item = "null";
                        addToSetIfNotContains(defSet, name);
                        addToSetIfNotContains(defSet, type);
                        addToSetIfNotContains(defSet, item);
                        sw.Write("gt::objinfo(" + TmxObjectToBnRectStr(tmxObject, topLeftCorner, ci) + ",Def::" + name + ",Def::" + type + ",Def::" + item + ")");
                        if (i < objInfoGroup.Objects.Count - 1)
                        {
                            sw.Write(",");
                        }
                        sw.WriteLine();
                    }
                    if(objInfoGroup.Objects.Count==0)
                        sw.WriteLine("gt::objinfo(bn::fixed_point(0,0),Def::null,Def::null,Def::null)");
                    sw.WriteLine("};");

                    //Write mapobjects
                    sw.WriteLine("constexpr const uint8_t " + mapname + "_mapobjects[] = {");
                    for (int i = 0; i < objectLayer.Tiles.Count; i++)
                    {
                        TmxLayerTile tile = objectLayer.Tiles[i];
                        TmxTilesetTile setTile = GetTileFromGid(map, tile.Gid);
                        int outputNum = 0;
                        if (setTile != null)
                        {
                            switch (setTile.Type)
                            {
                                case "collision": outputNum = 1; break;
                                case "enemy1": outputNum = 2; break;
                                case "enemy_bat": outputNum = 3; break;
                                case "warp": outputNum = 4;break;
                            }
                        }
                        if (i < objectLayer.Tiles.Count - 1)
                        {
                            sw.Write(outputNum.ToString() + ",");
                        }
                        else
                        {
                            sw.WriteLine(outputNum.ToString());
                        }
                    }
                    sw.WriteLine("};");
                }

                sw.WriteLine("#define mapinfo_count "+mapNamesWritten.Count);
                sw.WriteLine("const gt::map_info mapinfos[] = {");

                for(int i=0;i<mapNamesWritten.Count;i++)
                {
                    string mapname = mapNamesWritten[i];
                    addToSetIfNotContains(defSet, mapname);
                    sw.Write("gt::map_info(Def::" + mapname + ",bn::regular_bg_items::" + mapname + "," +
                        mapname + "_mapobjects," + mapname + "_areas," + mapname + "_area_count," +
                        mapname + "_signs," + mapname + "_sign_count," +
                        mapname + "_warps," + mapname + "_warp_count,"+
                        mapname + "_objinfos,"+mapname+ "_objinfo_count)");
                    if(i < mapNamesWritten.Count - 1)
                    {
                        sw.Write(",");
                    }
                    sw.WriteLine();
                }

                sw.WriteLine("};");

                sw.WriteLine("#endif");
            }

            using (StreamWriter sw = new StreamWriter(defFileName, false, Encoding.UTF8))
            {
                sw.WriteLine("#ifndef DEFINITIONS_H");
                sw.WriteLine("#define DEFINITIONS_H");
                sw.WriteLine();
                sw.WriteLine("enum class Def{");
                string[] names = new string[defSet.Count];
                defSet.CopyTo(names);
                for(int i=0;i<names.Length;i++)
                {
                    sw.Write(names[i]);
                    if(i<names.Length-1)
                    {
                        sw.Write(",");
                    }
                    sw.WriteLine();
                }
                sw.WriteLine("};");
                sw.WriteLine("#endif");
            }

        }
    }
}
