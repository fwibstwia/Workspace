-- file: ch01/wchs.hs

main = interact wordCount
 where wordCount input = show (length (lines input)) ++ "\n"