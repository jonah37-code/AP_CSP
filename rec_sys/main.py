# Database
meals_data = [
    {"meal_id": 1, "name": "Grilled Chicken Bowl", "calories": 520, "tags": ["high-protein", "healthy", "low-carb"]},
    {"meal_id": 2, "name": "Spaghetti Bolognese", "calories": 780, "tags": ["comfort", "high-carb"]},
    {"meal_id": 3, "name": "Salmon Rice Bowl", "calories": 600, "tags": ["high-protein", "omega-3", "healthy"]},
    {"meal_id": 4, "name": "Cheeseburger", "calories": 900, "tags": ["fast-food", "high-fat"]},
    {"meal_id": 5, "name": "Greek Yogurt Parfait", "calories": 300, "tags": ["healthy", "high-protein", "breakfast"]},
    {"meal_id": 6, "name": "Chicken Alfredo", "calories": 850, "tags": ["comfort", "high-fat", "high-carb"]},
    {"meal_id": 7, "name": "Avocado Toast", "calories": 350, "tags": ["healthy", "breakfast", "low-carb"]},
    {"meal_id": 8, "name": "Beef Stir Fry", "calories": 650, "tags": ["high-protein", "healthy", "asian"]},
    {"meal_id": 9, "name": "Tuna Salad", "calories": 400, "tags": ["high-protein", "low-carb", "healthy"]},
    {"meal_id": 10, "name": "Pancakes", "calories": 700, "tags": ["breakfast", "high-carb", "sweet"]},
    {"meal_id": 11, "name": "Chicken Wrap", "calories": 550, "tags": ["high-protein", "portable"]},
    {"meal_id": 12, "name": "Veggie Stir Fry", "calories": 450, "tags": ["vegetarian", "healthy", "low-fat"]},
    {"meal_id": 13, "name": "Protein Smoothie", "calories": 250, "tags": ["high-protein", "healthy", "drink"]},
    {"meal_id": 14, "name": "Mac and Cheese", "calories": 820, "tags": ["comfort", "high-carb", "cheesy"]},
    {"meal_id": 15, "name": "Turkey Sandwich", "calories": 500, "tags": ["high-protein", "lunch"]},
    {"meal_id": 16, "name": "Sushi Rolls", "calories": 600, "tags": ["asian", "healthy", "low-fat"]},
    {"meal_id": 17, "name": "Omelette", "calories": 420, "tags": ["breakfast", "high-protein", "healthy"]},
    {"meal_id": 18, "name": "Protein Bar", "calories": 220, "tags": ["high-protein", "snack"]}
]

users_data = [
    {"user_id": 1, "name": "Alex"},
    {"user_id": 2, "name": "Jordan"},
    {"user_id": 3, "name": "Taylor"},
    {"user_id": 4, "name": "Casey"},
    {"user_id": 5, "name": "Riley"}
]
ratings_data = [

    {"user_id": 1, "meal_id": 1, "rating": 5},
    {"user_id": 1, "meal_id": 3, "rating": 4},
    {"user_id": 1, "meal_id": 9, "rating": 5},

    {"user_id": 2, "meal_id": 2, "rating": 5},
    {"user_id": 2, "meal_id": 6, "rating": 4},
    {"user_id": 2, "meal_id": 14, "rating": 5},

    {"user_id": 3, "meal_id": 5, "rating": 5},
    {"user_id": 3, "meal_id": 7, "rating": 4},
    {"user_id": 3, "meal_id": 17, "rating": 5},

    {"user_id": 4, "meal_id": 4, "rating": 5},
    {"user_id": 4, "meal_id": 14, "rating": 4},
    {"user_id": 4, "meal_id": 10, "rating": 3},

    {"user_id": 5, "meal_id": 8, "rating": 5},
    {"user_id": 5, "meal_id": 12, "rating": 4},
    {"user_id": 5, "meal_id": 16, "rating": 5}
]

def main():
    user_id = get_user_id()
    recommendations = get_recommendations(user_id)

# Function that returns a validated User ID
def get_user_id():
    while True:
        try:
            user_id = int(input("Enter a User ID: "))
        except:
            print(f"Please enter a number.\n")
            continue
        
        if 1 <= user_id <= len(users_data): return user_id
        else: print(f"Please enter a number between 1 and {len(users_data)}.\n")

# Function that retruns a list of recommended meals
def get_recommendations(user_id):
    # Identify meals that user has tried and meals tried by other users
    meals_tried_by_user = []
    meals_tried_by_other_users = []
    for rating in ratings_data:
        if rating["user_id"] == user_id:
            meals_tried_by_user.append(rating)
        else:
            meals_tried_by_other_users.append(rating)

    # Identify meals the user has not tried
    new_meals = meals_data
    for tried_meal in sorted(meals_tried_by_user, key=lambda tried_meal: tried_meal["meal_id"], reverse=True):
        del new_meals[tried_meal["meal_id"] - 1] # Assumes that the meals are arranged in meals_data according to their meal id and that no meals are missing

    # Give every meal that the user has not tried a content-based score
    for new_meal in new_meals:
        content_based_score = 0
        for tried_meal in meals_tried_by_user:
            meal_coefficient = 1.5 ** tried_meal["rating"] - 3
            similarities = len(set(meal["tags"]) & set(meals_data[preference["meal_id"]]["tags"]))
            content_based_score += similarites * meal_coefficient
        new_meal["content_based_score"] = content_based_score


    # 

    # Give every meal the user has not tried a collaborative score
    for untried_meal in meals_tried_by_other_users:
        collaborative_score = 0
        for tried_meal in meals_tried_by_user:
            if untried_meal["meal_id"] == tried_meal["meal_id"]:

        new_meal["collaborative_score"] = collaborative_score

if __name__ == "__main__":
    main()
