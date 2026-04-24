def main():
    meals, users, ratings = load_data()
    user_id = get_user_id(users)

    tried_meals = get_tried_meals(user_id, ratings, meals)
    untried_meals = get_untried_meals(tried_meals, meals)

    similarity_scores = compute_similarity_scores(user_id, users, tried_meals, ratings)
    compute_content_scores(untried_meals, tried_meals)
    compute_collaborative_scores(user_id, ratings, similarity_scores, untried_meals)
    top_recommendations = get_recommendations(untried_meals)

    output_top_recommendations(top_recommendations, meals)

def load_data():

    # Meal, user, and rating sample data was generated using ChatGPT.
    meals = [
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

    users = [
        {"user_id": 1, "name": "Alex"},
        {"user_id": 2, "name": "Jordan"},
        {"user_id": 3, "name": "Taylor"},
        {"user_id": 4, "name": "Casey"},
        {"user_id": 5, "name": "Riley"}
    ]

    ratings = [

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

    return meals, users, ratings

def get_user_id(users):
    while True:
        user_name = input("Enter your user name: ")
        for user in users:
            if user["name"] == user_name:
                return user["user_id"]
        print("Invalid user name. Please try again.\n")

def get_tried_meals(user_id, ratings, meals):
    tried_meals = []
    for rating in ratings:
        if rating["user_id"] == user_id:
            meal_id = rating["meal_id"]
            meal_object = meals[meal_id - 1].copy()
            meal_object["rating"] = rating["rating"]
            tried_meals.append(meal_object)
    return tried_meals

def get_untried_meals(tried_meals, meals):
    untried_meals = []
    for meal in meals:
        if meal["meal_id"] not in [tried_meal["meal_id"] for tried_meal in tried_meals]:
            untried_meals.append(meal)
    return untried_meals

def compute_content_scores(untried_meals, tried_meals):
    for untried_meal in untried_meals:
        content_score = 0
        for tried_meal in tried_meals:
            rating_weight = 1.5 ** tried_meal["rating"] - 3
            similarity_count = len(set(tried_meal["tags"]) & set(untried_meal["tags"]))

            content_score += similarity_count * rating_weight
        untried_meal["content_score"] = content_score

def compute_similarity_scores(user_id, users, tried_meals, ratings):
    similarity_score_array = [0.0] * len(users)
    for user in [user for user in users if user["user_id"] != user_id]:
        total = 0
        common_meal_counter = 0

        for rating in [rating for rating in ratings if rating["user_id"] == user["user_id"]]:
            for tried_meal in tried_meals:
                if rating["meal_id"] == tried_meal["meal_id"]:
                    user_rating = tried_meal["rating"]
                    other_user_rating = rating["rating"]
                    try:
                        total += min((user_rating/other_user_rating), (other_user_rating/user_rating))
                    except ZeroDivisionError:
                        total = 0

                    common_meal_counter += 1
        try:
            similarity_score_array[user["user_id"] - 1] = total/common_meal_counter
        except ZeroDivisionError:
            similarity_score_array[user["user_id"] - 1] = 0

    return similarity_score_array

def compute_collaborative_scores(user_id, ratings, similarity_scores, untried_meals):
    for untried_meal in untried_meals:
        untried_meal["collaborative_score"] = 0
    
    for rating in [rating for rating in ratings if rating["user_id"] != user_id and rating["meal_id"] in [untried_meal["meal_id"] for untried_meal in untried_meals]]:
        for untried_meal in [untried_meal for untried_meal in untried_meals if untried_meal["meal_id"] == rating["meal_id"]]:
            other_user_id = rating["user_id"]
            other_user_rating = rating["rating"]
            similarity_score = similarity_scores[other_user_id - 1]

            untried_meal["collaborative_score"] += other_user_rating * similarity_score

def get_recommendations(untried_meals):
    top_scores = []
    
    for untried_meal in untried_meals:
        score = untried_meal["content_score"] + untried_meal["collaborative_score"]
        top_scores.append({"meal_id": untried_meal["meal_id"], "score": score})

    top_scores = sorted(top_scores, key=lambda meal: meal["score"], reverse=True)[:3] 

    return top_scores

def output_top_recommendations(top_meal_scores, meals):
    print(f"Your top 3 meal recommendations are:")
    for index, meal in enumerate(reversed(top_meal_scores)):
        print(f"    {index + 1}. {meals[meal["meal_id"] - 1]["name"]}")

if __name__ == "__main__":
    main()
