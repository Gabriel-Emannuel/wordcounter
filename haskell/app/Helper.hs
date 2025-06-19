module Helper where

import System.Directory (listDirectory, doesDirectoryExist, doesFileExist)
import Control.Monad (filterM)
import Data.List ()

addPrefix :: FilePath -> [FilePath] -> [FilePath]
addPrefix prefix = map (\entry -> prefix ++ "/" ++ entry)

takeAllDirs :: FilePath -> IO [FilePath]
takeAllDirs path = do
    entries <- listDirectory path
    directories <- (filterM doesDirectoryExist . addPrefix path) entries
    directoriesStack <- mapM takeAllDirs directories
    return $ directories ++ concat directoriesStack

takeAllFiles :: [FilePath] -> IO [FilePath]
takeAllFiles dirspaths = do
    entriesPerDir <- mapM listDirectory dirspaths
    (filterM doesFileExist . concat . zipWith addPrefix dirspaths) entriesPerDir