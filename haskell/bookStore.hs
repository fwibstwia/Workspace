data BookInfo = Book Int String [String]
              deriving (Show)
myInfo = Book 980 "Algebra of Programming" ["Richard Bird"]

type CustomerID = Int
type ReviewBody = String

data BetterReview = BetterReview BookInfo CustomerID ReviewBody

type BookRecord = (BookInfo, BetterReview)

bookId (Book id title authors) = id

data Customer = Customer {
  customerID :: Int,
  customerName :: String,
  customerAddress :: [String]
  } deriving (Show)