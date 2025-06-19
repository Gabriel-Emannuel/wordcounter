module Main where

import System.Environment(getArgs)

import Data.List.Split (splitOn)

import System.Directory ()

import System.IO (readFile)

import Helper (takeAllDirs, takeAllFiles)

import Control.Concurrent (newEmptyMVar, newMVar, takeMVar, forkIO)

limitThread = 4

wcFile :: FilePath -> IO Int
wcFile filepath = do
    content <- readFile filepath
    return $ (length . splitOn " ") content

paralel :: [FilePath] -> IO ()
paralel files = do
    sumParalel <- paralelAux files
    printFiles sumParalel

paralelAux :: [FilePath] -> IO Int
paralelAux []    = return 0
paralelAux files = do
    pool <- (mapM (forkIO . newMVar . wcFile) . take limitThread) files
    values <- mapM takeMVar pool
    stackParalel <- ( paralelAux . drop limitThread ) files
    return $ sum values + stackParalel

serial :: [FilePath] -> IO ()
serial files = do
    wcFiles <- mapM wcFile files
    printFiles $ sum wcFiles

printFiles :: Int -> IO ()
printFiles value = putStrLn $ "Existem " ++ show value ++ " palavras no dataset"

main :: IO ()
main = do
    args <- getArgs
    case args of
        [dir, mode] -> do
            dirs <- takeAllDirs dir
            files <- takeAllFiles dirs
            if mode == "Serial"
                then serial files
                else if mode == "Paralel"
                    then paralel files
                    else putStrLn "Usage haskell-tread <dataset dir> <Serial|Paralel>"
        _ -> putStrLn "Usage haskell-tread <dataset dir> <Serial|Paralel>"